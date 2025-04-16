#include"userServer.h"
using namespace net;

UserServer::UserServer()
          : addr_(8080),
            server_(&loop_,addr_)
{
  server_.setThreadNum(3);
  server_.setConnectionCallback([this](const TcpConnectionPtr & conn){ onConnection(conn); });
  server_.setMessageCallback([this](const TcpConnectionPtr & conn,Buffer * buff,Timestamp time){ onMessage(conn,buff,time); });
  server_.start();
}

void UserServer::onConnection(const net::TcpConnectionPtr & conn) {

}

void UserServer::onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time) {
  
}