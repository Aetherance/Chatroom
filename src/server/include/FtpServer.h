#include"TcpServer.h"
#include"logger.h"
#include<filesystem>

using namespace ilib;
using namespace ilib::net;
using namespace ilib::base;

class FtpServer
{
friend class ServiceHandler;
public:
  FtpServer();

  void run();

  ~FtpServer();

private:
  void onConnection(const TcpConnectionPtr & conn);
  
  void onMessage(const TcpConnectionPtr & conn, Buffer * buf, Timestamp time);
  
  void parseMessage(const std::string & msg, const TcpConnectionPtr & conn);

  void handleUpload(const std::string & fileName, const std::string & userDir, const TcpConnectionPtr & conn);

  void handleDownload(const std::string & fileName, const std::string & userDir, const TcpConnectionPtr & conn);

  void sendResponse(const std::string & Msg,const net::TcpConnectionPtr & conn);

  void onReceive(const std::string & dir , const std::string & fileName, const TcpConnectionPtr & conn,const int sockfd);

  void onSend(const std::string & dir , const std::string & fileName, const TcpConnectionPtr & conn,const int sockfd);

  InetAddress address_;
  TcpServer server_;
  EventLoop loop_;

  std::filesystem::path root_;
};