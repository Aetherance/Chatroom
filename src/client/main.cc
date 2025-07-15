#include"client.h"
#include"sig.h"
#include<iostream>

int main(int argc,char *argv[]) {
  if(argc == 1) {
    Client client("localhost");
    client.run();
  } else {
    Client client(argv[1]);
    client.run();
  }
  

  return 0;
}