#include"Socket.h"
#include<filesystem>

using namespace ilib;
using namespace ilib::net;

class FtpClient
{
public:
  FtpClient();

  void connect();

  void uploadFile(const std::string &filePath, const std::string &remoteDir , const std::string &filename);

  void downloadFile(const std::filesystem::path, const std::string &remoteDir , const std::string &filename);

  void safeSend(const std::string & message);

  std::string safeRecv();

  ~FtpClient();

private:
  Socket controlSocket_;
  InetAddress serverAddr_;
};