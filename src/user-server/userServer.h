#include"TcpServer.h"
#include<memory>
using namespace ilib;

#define SERVER_PORT 8080

class UserServer
{
public:
  UserServer();
  void run();
private:
  /* addr_必须先于server声明 */
  net::InetAddress addr_;

  net::TcpServer server_;
  net::EventLoop loop_;

  void onConnection(const net::TcpConnectionPtr & conn);
  void onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time);
};