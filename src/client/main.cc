#include"Client.h"
#include<iostream>

int main() {
  // UserClient userClient;
  // userClient.Connect();
  // std::cout<<"正在注册! 请输入邮箱 用户名和密码!";
  // std::string email,username,passwd;
  // std::cin>>email>>username>>passwd;
  // userClient.RequestRegister(email,username,passwd);

  // userClient.RequestLogin("2085163736@qq.com","123");

  Client client;
  client.run();


  getchar();

  return 0;
}