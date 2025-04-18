#include"UserClient.h"
#include<iostream>

int main() {
  UserClient userClient;
  userClient.Connect();
  userClient.RequestRegister("2085163736@qq.com");

  getchar();

  return 0;
}