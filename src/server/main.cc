#include"UserServer.h"
#include"ChatServer.h"
#include"FtpServer.h"
#include<thread>

int main() {
  ChatServer chatServer;

  std::thread user([&]{
    UserServer userServer;
    userServer.run();
  });

  std::thread ftp([&]{ 
    FtpServer ftpServer;
    ftpServer.run();
  });

  chatServer.run();  

  user.join();
  ftp.join();  

  return 0;
}