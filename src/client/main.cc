#include"client.h"
#include"sig.h"
#include<iostream>

int main(int argc,char *argv[]) {
  if(argc == 3) {
    if( std::string(argv[2]) == "--tui=disable") {
      Client client(argv[1]);
      client.run_without_ui();
    }

    return 0;
  }
  
  if(argc == 1) {
    printf(" 错误! 未指定服务器IP地址\n");
    return -1;
  } else {
    static Client client(argv[1]);
    client.run();
  }

  return 0;
}