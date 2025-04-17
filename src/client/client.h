#ifndef CLIENT_H
#define CLIENT_H

// 服务器IP地址和段口号
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

#include"../netlib/net/Socket.h"
#include"buff.h"

class Client
{
public:
  Client();
  ~Client() {}

  void Connect();
  void Send(Buff buff);
  std::string Recv();
private:
  ilib::net::Socket sock_;
  Buff buff_;
};

#endif