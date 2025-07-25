#include"UserServer.h"
#include"ChatServer.h"
#include"FtpServer.h"
#include<thread>

int main() {
  FtpServer ftp;

  ftp.run();

  return 0;
}