#include"ChatServer.h"
#include"msg.pb.h"
#include"logger.h"

using namespace net;

#define CHAT_SERVER_REACTOR_NUM_ 16
#define TCP_HEAD_LEN sizeof(int)

std::unordered_map<std::string,std::string> AddrHashOnlineUser;
std::unordered_map<std::string,net::TcpConnectionPtr> userHashConn;

ChatServer::ChatServer() : addr_(7070),
                         server_(&loop_,addr_)
{
  server_.setThreadNum(CHAT_SERVER_REACTOR_NUM_);
  server_.setConnectionCallback([this](const TcpConnectionPtr & conn){ onConnection(conn); });
  server_.setMessageCallback([this](const TcpConnectionPtr & conn,Buffer* buff,Timestamp time){ onMessage(conn,buff,time); });
  
  redis_.connect("127.0.0.1",6379);
}

void ChatServer::run() {
  LOG_INFO("The Chat Server is starting up ...");
  server_.start();

  std::string success = "The Chat Server is running on " + addr_.toIpPort() + " now !";
  LOG_INFO_SUCCESS(success);

  loop_.loop();
}

void ChatServer::onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time) {
  // TCP粘包处理
  while(buff->readableBytes() >= TCP_HEAD_LEN) {
    const char * data = buff->peek();
    int headLen = ntohl( *(int *)data );
    if(buff->readableBytes() >= headLen + TCP_HEAD_LEN) {
      buff->retrieve(TCP_HEAD_LEN);
      std::string msg(buff->peek(),headLen);
      LOG_INFO(headLen + " bytes was forwarded by the server!");
      parseMessage(msg,conn);
      buff->retrieve(headLen);
    } else {
      break;
    }
  }
}

void ChatServer::parseMessage(const std::string & msg_str,const net::TcpConnectionPtr & conn) {
  Message msg;
  LOG_INFO("A message is about to send!");
  if(!msg.ParseFromString(msg_str)) {
    perror("Protobuf: ChatServer");
    return ;
  }
  
  bool isExists = false;
  redis_.sismember(allUserset,msg.to(),[&isExists](cpp_redis::reply & reply){ isExists = reply.as_integer(); });
  redis_.sync_commit();
  
  if(isExists == false) {
    LOG_ERROR("parseMessage: User Not Found!");
    return ;
  }

  bool isOnline = false;
  redis_.sismember(onlineUserSet,msg.to(),[&isOnline](cpp_redis::reply & reply){ isOnline = reply.as_integer(); });
  redis_.sync_commit();

  if(isOnline == false) {
    LOG_ERROR("parseMessage: User is offline now!");
    return ;
  }

  LOG_INFO(msg.from() + " says \n" + msg.text() + "\n to " + msg.to());

  assert(userHashConn[msg.to()]);
  sendMsgToUser(msg_str,userHashConn[msg.to()]);
};

void ChatServer::onConnection(const net::TcpConnectionPtr & conn) {
  /* DEBUG */
  AddrHashOnlineUser[conn->peerAddress().toIpPort()] = "op";
  /* DEBUG */
  
  if( conn->connected()) {
    assert(conn);
    LOG_INFO(AddrHashOnlineUser[conn->peerAddress().toIpPort()]);
    userHashConn[AddrHashOnlineUser[conn->peerAddress().toIpPort()]] = conn;
  } else if( !conn->connected()) {
    // redis_.srem(onlineUserSet,{AddrHashOnlineUser[conn->peerAddress().toIpPort()]});
    // redis_.sync_commit();
    LOG_INFO("User " + AddrHashOnlineUser[conn->peerAddress().toIpPort()] + " on " + conn->peerAddress().toIpPort() + " left!");
    AddrHashOnlineUser[conn->peerAddress().toIpPort()] = "";
  }
}

void ChatServer::sendMsgToUser(const std::string & Msg,const net::TcpConnectionPtr & conn) {
  Buffer sendBuff;

  int HeadLen = htonl(Msg.size());
  
  sendBuff.append(&HeadLen,sizeof(HeadLen));
  sendBuff.append(Msg);

  assert(conn);
  conn->send(sendBuff.retrieveAsString());
}