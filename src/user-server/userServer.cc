#include"userServer.h"
#include"logger.h"
using namespace net;

#define USER_SERVER_REACTOR_NUM_ 4


UserServer::UserServer()
          : addr_(SERVER_PORT),
            server_(&loop_,addr_)
{
  server_.setThreadNum(USER_SERVER_REACTOR_NUM_);
  server_.setConnectionCallback([this](const TcpConnectionPtr & conn){ onConnection(conn); });
  server_.setMessageCallback([this](const TcpConnectionPtr & conn,Buffer * buff,Timestamp time){ onMessage(conn,buff,time); });
}

void UserServer::onConnection(const net::TcpConnectionPtr & conn) {
  
}

void UserServer::onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time) {
  std::string logInfo = "receive a message from " + conn->peerAddress().toIpPort();
  LOG_INFO(logInfo);
  conn->send("hello!");
}

void UserServer::run() {
  LOG_INFO("The server is starting up...");
  server_.start();
  
  std::string success = "The server is running on " + addr_.toIpPort() + " now !";
  LOG_INFO_SUCCESS(success);
  
  loop_.loop();
}