#ifndef FTP_CLIENT
#define FTP_CLIENT

#include"Socket.h"
#include<filesystem>
#include<unordered_map>
#include<unordered_set>
#include<vector>

using namespace ilib;
using namespace ilib::net;

class FtpClient
{
friend class Client;
friend class MsgClient;
public:
  FtpClient();

  void connect();

  void uploadFile(const std::string &filePath, const std::string &remoteDir , const std::string &filename);

  void downloadFile(const std::filesystem::path, const std::string &remoteDir , const std::string &filename);

  void safeSend(const std::string & message);

  void updateProgress(std::string filename,uintmax_t ToltalBytes,uintmax_t TransedBytes);

  std::string safeRecv();

  ~FtpClient();

private:
  Socket controlSocket_;
  InetAddress serverAddr_;

  std::unordered_map<std::string,bool> transList;

  std::unordered_map<std::string,float> transProgressMap;
};

#endif