#include"client.h"
#include"InetAddress.h"
#include<iostream>
void Client::Connect() {
  sockaddr_in peerAddr = ilib::net::InetAddress(SERVER_IP,SERVER_PORT).getSockAddr();
  
  char buff[1203];
  inet_ntop(AF_INET,&peerAddr.sin_addr,buff,1203);
  std::cout<<buff;

  int stat = connect(sock_.fd(),reinterpret_cast<sockaddr*>(&peerAddr),sizeof(peerAddr));
  if(stat < 0) {
    perror("Client: connect");
  }
}

Client::Client()
  : sock_(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) 
{}