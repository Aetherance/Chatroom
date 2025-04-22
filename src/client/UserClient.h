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

  int SendRegister1(const std::string & email);
  int SendRegister2(const std::string email,const std::string & user_name,const std::string& passwd,const std::string & code);
  
  int RequestLogin(const std::string & email,const std::string & passwd);
  
  bool isConnected() { return isConnected_; } ;
private:
  void Send(std::string msg);
  int Recv();

  std::string ConstructRegister1(const std::string email);

  std::string ConstructRegister2(const std::string email,
              const std::string user_name,const std::string passwd,const std::string code);

  std::string ConstructLogin(const std::string & email,const std::string & passwd);


  void SendLogin(const std::string & email,const std::string & passwd);

  bool isConnected_;

  ilib::net::Socket sock_;
};

#endif