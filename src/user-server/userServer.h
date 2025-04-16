#include"../netlib/net/TcpServer.h"
#include<memory>
using namespace ilib;

class UserServer
{
public:
  UserServer();
private:
  net::TcpServer server_;
  net::EventLoop loop_;
  net::InetAddress addr_;

  void onConnection(const net::TcpConnectionPtr & conn);
  void onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time);
};