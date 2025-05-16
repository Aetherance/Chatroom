#include"FtpServer.h"

using namespace std::filesystem;

FtpServer::FtpServer() : addr_(6060),
                         server_(&loop_,addr_),
                         rootPath_("../root")
{
  create_directory(rootPath_);
  server_.setConnectionCallback([this](const net::TcpConnectionPtr& conn){ onConnection(conn); });
  server_.setMessageCallback([this](const net::TcpConnectionPtr &conn,net::Buffer * buff,base::Timestamp timestamp){ onMessage(conn,buff,timestamp); });
  server_.setThreadNum(3);
}

FtpServer::~FtpServer() {

}

void FtpServer::run() {
  server_.start();

  LOG_INFO_SUCCESS("The Ftp Server is running on port 6060!");

  loop_.loop();
}

void FtpServer::onConnection(const net::TcpConnectionPtr &) {
  
}

void FtpServer::onMessage(const net::TcpConnectionPtr &,net::Buffer * buff,base::Timestamp timestamp) {

}

bool FtpServer::isUserDirExist(const std::string & user) const {
  path userPath = rootPath_ / user;
  return exists(userPath) && is_directory(userPath);
}