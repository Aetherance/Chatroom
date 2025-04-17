#include"client.h"
#include"InetAddress.h"

void Client::Connect() {
  sockaddr_in peerAddr = ilib::net::InetAddress(SERVER_IP,SERVER_PORT).getSockAddr();
  
  int stat = connect(sock_.fd(),reinterpret_cast<sockaddr*>(&peerAddr),sizeof(peerAddr));
  if(stat < 0) {
    perror("Client: connect");
  }
}

Client::Client()
  : sock_(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) 
{}

void Client::Send(Buff buff) {
  buff.send(sock_);
}

std::string Client::Recv() {
  Buff buff;
  buff.recv(sock_);
  return buff.data();
}