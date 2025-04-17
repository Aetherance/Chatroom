#include"buff.h"

Buff::Buff(int size) {
  data_.reserve(size);
}

Buff& Buff::operator<<(ilib::net::Socket sock) {
  ::read(sock.fd(),data_.data(),BUFF_SIZE);

  return *this;
}

Buff& Buff::operator>>(ilib::net::Socket sock) {
  if(data_.empty()) {
    return *this;
  }
  ::write(sock.fd(),data_.data(),data_.size());

  return *this;
}