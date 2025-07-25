#include"UserServer.h"
#include"ChatServer.h"
#include"FtpServer.h"
#include<thread>

int main() {
  ChatServer chat;

  chat.run();

  return 0;
}