#include"FtpServer.h"
#include"fileInfo.pb.h"
#include"fcntl.h"

#define TCP_HEAD_LEN sizeof(uint32_t)

FtpServer::FtpServer()
    : address_(InetAddress(6060)),
      server_(&loop_, address_),
      root_("../root")
{
  server_.setConnectionCallback([this](const TcpConnectionPtr & conn){ onConnection(conn); });
  server_.setMessageCallback([this](const TcpConnectionPtr & conn, Buffer * buf, Timestamp time) { onMessage(conn, buf, time); });
  server_.setThreadNum(4);
  std::filesystem::create_directory(root_);
}

FtpServer::~FtpServer() {
  
}

void FtpServer::run() {
  server_.start();

  LOG_INFO_SUCCESS("The FtpServer is runnig on port 6060!");
  
  loop_.loop();
}

void FtpServer::onConnection(const TcpConnectionPtr & conn) {
  
}

void FtpServer::onMessage(const TcpConnectionPtr & conn, Buffer * buff, Timestamp time) {
  // TCP粘包处理
  constexpr uint32_t MAX_MSG_SIZE = 10 * 1024 * 1024;  // 10MB

  while(buff->readableBytes() >= TCP_HEAD_LEN) {
    const char * data = buff->peek();
    uint32_t headLen;
    memcpy(&headLen, data, sizeof(headLen));
    headLen = ntohl(headLen);

  if(buff->readableBytes() >= headLen + TCP_HEAD_LEN) {
      buff->retrieve(TCP_HEAD_LEN);
      LOG_INFO(std::to_string(headLen) + " bytes was forwarded by the server!");
      if (headLen <= 0 || headLen > MAX_MSG_SIZE) {
        LOG_ERROR("Invalid headLen: " + std::to_string(headLen));
        conn->shutdown();
        return;
      }
      
      std::string msg(buff->peek(),headLen);

      parseMessage(msg,conn);

      buff->retrieve(headLen);
    } else {
      break;
    }
  }
}

void FtpServer::parseMessage(const std::string & msg, const TcpConnectionPtr & conn) {
  fileInfo info;
  if (!info.ParseFromString(msg)) {
    LOG_ERROR("Failed to parse message from client");
    return;
  }

  if (info.action() == "UPLOAD") {
    handleUpload(info.file_name(), info.user_dir(), conn);
  } else if (info.action() == "DOWNLOAD") {
    handleDownload(info.file_name(), info.user_dir(), conn);
  } else {
    LOG_ERROR("Unknown action: " + info.action());
    sendResponse("ERROR: Unknown action", conn);
  }
}

unsigned int bindUsable(int sockfd) {
  sockaddr_in sin;
  memset(&sin,0,sizeof(sin));
  srand(time(NULL));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  unsigned int try_port;
  while (true) {
    try_port = rand() % (65535 - 1024 + 1) + 1024;
    sin.sin_port = htons(try_port);
    if (::bind(sockfd, (struct sockaddr *)&sin, sizeof(sin)) == 0) {
      return try_port;
    }
  }
}

void FtpServer::handleUpload(const std::string & fileName, const std::string & userDir, const TcpConnectionPtr & conn) {
  LOG_INFO("Received upload request for file: " + fileName + " in directory: " + userDir);
  
  int dataSockfd = ::socket(AF_INET,SOCK_STREAM,0);
  unsigned int port = bindUsable(dataSockfd);

  std::thread receiveThread([=](){ onReceive(userDir,fileName,conn,dataSockfd); });
  receiveThread.detach();

  fileInfo backInfo;
  backInfo.set_action("OK");
  backInfo.set_port(port);

  sendResponse(backInfo.SerializeAsString(), conn);
}

void FtpServer::handleDownload(const std::string & fileName, const std::string & userDir, const TcpConnectionPtr & conn) {
  LOG_INFO("Received download request for file: " + fileName + " in directory: " + userDir);
  
  std::filesystem::path filePath = root_ / userDir / fileName;

  if (!std::filesystem::exists(filePath)) {
    LOG_ERROR("File does not exist: " + filePath.string());
    sendResponse("ERROR: File does not exist", conn);
    return;
  }

  int dataSockfd = ::socket(AF_INET,SOCK_STREAM,0);
  unsigned int port = bindUsable(dataSockfd);

  std::thread sendThread([=](){ onSend(userDir,fileName,conn,dataSockfd); });
  sendThread.detach();

  fileInfo backInfo;
  backInfo.set_action("OK");
  backInfo.set_port(port);

  sendResponse(backInfo.SerializeAsString(), conn);
}

void FtpServer::sendResponse(const std::string & Msg,const net::TcpConnectionPtr & conn) {
  Buffer sendBuff;

  int HeadLen = htonl(Msg.size());
  
  sendBuff.append(&HeadLen,sizeof(HeadLen));
  sendBuff.append(Msg);

  assert(conn);

  std::string buffmsg = sendBuff.retrieveAsString();
  
  conn->send(buffmsg);
}

int setBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) return -1; // 错误处理

    flags &= ~O_NONBLOCK;

    return fcntl(sockfd, F_SETFL, flags);
}

void FtpServer::onReceive(const std::string & dir , const std::string & fileName, const TcpConnectionPtr & conn,const int dataListenSockfd) {
  InetAddress peerAddr;
  Socket dataListenSocket(dataListenSockfd);
  dataListenSocket.listen();
  int dataSocket = dataListenSocket.accept(&peerAddr);

  setBlocking(dataSocket);

  if(dataSocket != -1) {
    LOG_INFO_SUCCESS("The Client has connected to the data transport port\ntransport begin!");
  }

  std::string storDir = std::filesystem::path(dir).parent_path();

  std::filesystem::create_directory(root_/storDir);

  std::filesystem::create_directory(root_/dir);

  std::ofstream file(root_/dir/fileName,std::ios::binary);

  LOG_INFO("Receiving file " + fileName);

  while (true) {
    std::vector<char>buff = {};
    buff.resize(1024);
    int n = ::read(dataSocket,buff.data(),buff.size());
    if(n <= 0) {
      perror("read");
      break;
    }
    file.write(buff.data(),n);
  }

  file.close();
  ::close(dataSocket);
}

void FtpServer::onSend(const std::string & dir , const std::string & fileName, const TcpConnectionPtr & conn,const int dataListenSockfd) {
  InetAddress peerAddr;
  Socket dataListenSocket(dataListenSockfd);
  dataListenSocket.listen();
  int dataSocket = dataListenSocket.accept(&peerAddr);

  setBlocking(dataSocket);

  if(dataSocket != -1) {
    LOG_INFO_SUCCESS("The Client has connected to the data transport port\ntransport begin!");
  }

  std::filesystem::path filePath = root_/dir/fileName;

  if (!std::filesystem::exists(filePath)) {
    LOG_ERROR("File does not exist: " + filePath.string());
    return;
  }

  std::ifstream file(filePath, std::ios::binary);
  
  if (!file.is_open()) {
    LOG_ERROR("Failed to open file: " + filePath.string());
    return;
  }

  LOG_INFO("Sending file " + fileName);

  while (file) {
    std::vector<char> buff(1024);
    file.read(buff.data(), buff.size());
    std::streamsize bytesRead = file.gcount();
    
    if (bytesRead > 0) {
      ::send(dataSocket, buff.data(), bytesRead, 0);
    }
  }

  file.close();
  ::close(dataSocket);
}