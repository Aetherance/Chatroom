#include"UserServer.h"
#include"ChatServer.h"
#include"FtpServer.h"
#include<thread>

#include <gperftools/profiler.h>

void signalHandler(int signum)
{
  std::cout << "\n 捕获到 Ctrl+C (SIGINT)，停止 profiler\n";
  ProfilerStop();
  exit(signum);
}

int main() {

  ::signal(SIGINT, signalHandler);
  ProfilerStart("cpu.prof");

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