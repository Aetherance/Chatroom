#include"ChatServer.h"
#include"msg.pb.h"
#include"logger.h"

using namespace net;

#define CHAT_SERVER_REACTOR_NUM_ 16
#define TCP_HEAD_LEN sizeof(int)

std::unordered_map<std::string,net::TcpConnectionPtr> userHashConn;

ChatServer::ChatServer() : addr_(7070),
                           server_(&loop_,addr_),
                           serviceHandler_(this)
{
  server_.setThreadNum(CHAT_SERVER_REACTOR_NUM_);
  server_.setConnectionCallback([this](const TcpConnectionPtr & conn){ onConnection(conn); });
  server_.setMessageCallback([this](const TcpConnectionPtr & conn,Buffer* buff,Timestamp time){ onMessage(conn,buff,time); });
  
  redis_.connect("127.0.0.1",6379);
}

void ChatServer::run() {
  server_.start();

  std::string success = "The Chat Server is running on port 7070";
  LOG_INFO_SUCCESS(success);

  loop_.loop();
}

void ChatServer::onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time) {
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

void ChatServer::onConnection(const net::TcpConnectionPtr & conn) {
  if( conn->connected()) {
    assert(conn);
  } else if( !conn->connected()) {
    redis_.srem(onlineUserSet,{conn->user_email()});
    redis_.sync_commit();
    serviceHandler_.FriendBeOffline(conn);
    LOG_INFO("User " + conn->user_email() + " on " + conn->peerAddress().toIpPort() + " left!");
  }
}