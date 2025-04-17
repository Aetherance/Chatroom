#include"buff.h"

Buff::Buff(int size) {
  data_.resize(size);
}

Buff::Buff(const std::string &str) {
  data_.resize(BUFF_SIZE);
  data_.append(str);
}

Buff& Buff::operator<<(ilib::net::Socket &sock) {
  Buff::recv(sock);

  return *this;
}

Buff& Buff::operator>>(ilib::net::Socket &sock) {
  if(data_.empty()) {
    return *this;
  }
  Buff::send(sock);

  return *this;
}

void Buff::recv(ilib::net::Socket &sock) {
  int n = ::read(sock.fd(),data_.data(),BUFF_SIZE);
  if(n <= 0) {
    perror("::read");
  }
}

void Buff::send(ilib::net::Socket &sock) {
  int n = ::write(sock.fd(),data_.data(),data_.size());
  if(n <= 0) {
    perror("::write");
  }
}