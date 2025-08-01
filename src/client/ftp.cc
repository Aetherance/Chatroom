#include"ftp.h"
#include"fileInfo.pb.h"
#include<fstream>
#include"Buffer.h"
#include"fcntl.h"
#include<filesystem>
#include<ftxui/component/component.hpp>
#include<ftxui/dom/elements.hpp>
#include<ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>

extern ftxui::ScreenInteractive fileTranScreen;

FtpClient::FtpClient(const std::string & ip) : controlSocket_(::socket(AF_INET,SOCK_STREAM,0)),
                                               serverAddr_(InetAddress(ip, 6060)),
                                               ip_(ip)
{
  connect();
  std::filesystem::create_directory("./download");
}

FtpClient::~FtpClient() {

}

void FtpClient::connect() {
  ::connect(controlSocket_.fd(), (struct sockaddr *)&serverAddr_, sizeof(serverAddr_));
}

void FtpClient::uploadFile(const std::string &filePath, const std::string &remoteDir , const std::string &filename) {
  fileInfo info;
  info.set_action("UPLOAD");
  info.set_file_name(filename);
  info.set_user_dir(remoteDir);

  safeSend(info.SerializeAsString());

  std::string backMessage = safeRecv();

  fileInfo backInfo;
  backInfo.ParseFromString(backMessage);

  Socket dataSocket(::socket(AF_INET,SOCK_STREAM,0));
  InetAddress PeerAddr(ip_,backInfo.port());
  sockaddr_in sin = PeerAddr.getSockAddr();
  int stat = ::connect(dataSocket.fd(),(sockaddr*)&sin,sizeof(sin));
  if(stat < 0) {
    perror("::conncect");
  }
 
  int fd = ::open(filePath.data(),O_RDONLY);
  
  uintmax_t TransedBytes = 0;
  uintmax_t TotalBytes = std::filesystem::file_size(filePath);

  while (true) {
    std::vector<char> buff;
    buff.resize(1024);

    int n = ::read(fd,buff.data(),1024);

    if(n <= 0) {
      break;
    }

    ::send(dataSocket.fd(),buff.data(),n,0);

    TransedBytes += n;

    updateProgress(filename,TotalBytes,TransedBytes);
  }

  if(TotalBytes == TransedBytes) {
    transList[filename] = "传输完成";
    sleep(1);
    transList.erase(filename);
  } else {
    transList[filename] = "传输失败";
  }

  fileTranScreen.PostEvent(ftxui::Event::Custom);
}

void FtpClient::safeSend(const std::string & message) {
  uint32_t msg_len = static_cast<uint32_t>(message.size());
  uint32_t LengthHead = htonl(msg_len);
  ::send(controlSocket_.fd(), &LengthHead, sizeof(LengthHead), 0);
  ::send(controlSocket_.fd(),message.data(),message.size(),0);
}

std::string FtpClient::safeRecv() {
  int messageLength;
  std::string message;
  ::recv(controlSocket_.fd(),&messageLength,sizeof(messageLength),0);
  message.resize(messageLength);
  ::recv(controlSocket_.fd(),message.data(),message.size(),0);
  return message;
}

void FtpClient::downloadFile(const std::filesystem::path fileDir, const std::string &remoteDir , const std::string &filename) {
  const std::string filePath = fileDir/filename;
  fileInfo info;
  info.set_action("DOWNLOAD");
  info.set_file_name(filename);
  info.set_user_dir(remoteDir);

  safeSend(info.SerializeAsString());

  std::string backMessage = safeRecv();

  fileInfo backInfo;
  backInfo.ParseFromString(backMessage);

  Socket dataSocket(::socket(AF_INET,SOCK_STREAM,0));
  InetAddress PeerAddr(ip_,backInfo.port());
  sockaddr_in sin = PeerAddr.getSockAddr();
  int stat = ::connect(dataSocket.fd(),(sockaddr*)&sin,sizeof(sin));
  if(stat < 0) {
    perror("::conncect");
    return;
  }

  int fd = ::open(filePath.data(),O_WRONLY | O_CREAT | O_TRUNC,0644);
  
  int n = -1;
  uintmax_t TransedBytes = 0;
  uintmax_t TotalBytes = backInfo.size();

  while (true) {
    std::vector<char> buff;
    buff.resize(1024);

    n = ::recv(dataSocket.fd(),buff.data(),1024,0);

    if(n <= 0) {
      break;
    }

    ::write(fd,buff.data(),n);
    TransedBytes += n;
    updateProgress(filename,TotalBytes,TransedBytes);
  }

  if(TotalBytes == TransedBytes) {
    transList[filename] = "传输完成";
    sleep(1);
    transList.erase(filename);
  } else {
    transList[filename] = "传输失败";
  }
}

void FtpClient::updateProgress(std::string filename,uintmax_t TotalBytes,uintmax_t TransedBytes) {
  float progress = (TransedBytes * 1.0) / (TotalBytes * 1.0);
  transProgressMap[filename] = progress;
  fileTranScreen.PostEvent(ftxui::Event::Custom);
}