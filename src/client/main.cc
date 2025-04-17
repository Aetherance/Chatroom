#include"client.h"
#include<iostream>

int main() {
  Client client;
  client.Connect();
  client.Send("hello server!");
  std::cout<<"发送了一条消息!\n";

  std::cout<<client.Recv();

  return 0;
}