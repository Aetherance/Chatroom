#include<TcpServer.h>
#include"logger.h"
#include<filesystem>

using namespace ilib;

class FtpServer
{  
public:
  FtpServer();
  ~FtpServer();
  void run();

private:
  net::EventLoop loop_;
  net::InetAddress addr_;
  net::TcpServer server_;

  std::filesystem::path rootPath_;

  void onConnection(const net::TcpConnectionPtr &);
  
  void onMessage(const net::TcpConnectionPtr &,net::Buffer * buff,base::Timestamp timestamp);

  bool isUserDirExist(const std::string & user) const;
};