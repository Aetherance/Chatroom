#include"UserClient.h"
#include<iostream>

int main() {
  UserClient userClient;
  userClient.Connect();
  std::cout<<"正在注册! 请输入邮箱 用户名和密码!";
  std::string email,username,passwd;
  std::cin>>email>>username>>passwd;
  userClient.RequestRegister(email,username,passwd);

  getchar();

  return 0;
}