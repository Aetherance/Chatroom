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

  void RequestRegister(const std::string email,const std::string & user_name,const std::string passwd);
  void RequestLogin();
  
private:
  void Send(std::string msg);
  int Recv();

  /* 构造REGISTER1 报文 */
  std::string ConstructRegister1(const std::string email);

  std::string ConstructRegister2(const std::string email,
              const std::string user_name,const std::string passwd,const std::string code);

  void SendRegister1(const std::string & email);
  
  void SendRegister2(const std::string email,
              const std::string & user_name,const std::string& passwd);

  ilib::net::Socket sock_;
};

#endif