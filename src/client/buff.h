#ifndef BUFF_CLIENT_H
#define BUFF_CLIENT_H

#define BUFF_SIZE 64 * 1024

#include"string"
#include"Socket.h"

class Buff
{
public:
  Buff(int size = BUFF_SIZE);
  Buff(const std::string &str);
  Buff(const char str[]) { *this = Buff(std::string(str)); }

  Buff& operator>>(ilib::net::Socket& sock);
  Buff& operator<<(ilib::net::Socket& sock);

  void recv(ilib::net::Socket& sock);
  void send(ilib::net::Socket& sock);

  std::string data() { return data_; };

  ~Buff() {}
private:
  std::string data_;
};

#endif