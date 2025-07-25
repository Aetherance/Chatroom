#include"UserServer.h"
#include"ChatServer.h"
#include"FtpServer.h"
#include<thread>

int main() {
  UserServer user;

  user.run();

  return 0;
}