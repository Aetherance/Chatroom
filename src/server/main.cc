#include"UserServer.h"
#include"ChatServer.h"
#include"FtpServer.h"
#include<thread>

int main() {
  std::thread user([&]{
    UserServer userServer;
    userServer.run();
  });

  std::thread chat([&]{
    ChatServer chatServer;
    chatServer.run();
  });

  // FtpServer ftpServer;
  // ftpServer.run();

  user.join();
  chat.join();
  

  return 0;
}