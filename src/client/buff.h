#ifndef BUFF_CLIENT_H
#define BUFF_CLIENT_H

#define BUFF_SIZE 64 * 1024

#include"string"
#include"Socket.h"

class Buff
{
public:
  Buff(int size = BUFF_SIZE);
  Buff& operator>>(ilib::net::Socket sock);
  Buff& operator<<(ilib::net::Socket sock);
  ~Buff() {}
private:
  std::string data_;
};

#endif