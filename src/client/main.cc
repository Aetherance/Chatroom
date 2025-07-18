#include"client.h"
#include"sig.h"
#include<iostream>
#include <gperftools/profiler.h>

int main(int argc,char *argv[]) {
  if(argc == 1) {
    printf(" 错误! 未指定服务器IP地址\n");
    return -1;
  } else {
    Client client(argv[1]);
    client.run();
  }

  return 0;
}