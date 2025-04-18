#ifndef CLIENT_H
#define CLIENT_H

// 服务器IP地址和段口号
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

#include"Socket.h"

class UserClient
{
public:
  UserClient();
  ~UserClient() { sock_.shutdownWrite(); }

  void Connect();

  void RequestRegister(const std::string & email);
  void RequestLogin();
  
private:
  void Send(std::string msg);
  std::string Recv();

  std::string ConstructBindMessage(const std::string & email);

  void SendBindRequest(const std::string & email);
  void FinishRegister();

  ilib::net::Socket sock_;
};

#endif