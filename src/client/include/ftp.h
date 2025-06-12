#include"Socket.h"
#include<filesystem>
#include<unordered_map>
#include<unordered_set>

using namespace ilib;
using namespace ilib::net;

class FtpClient
{
friend class Client;
public:
  FtpClient();

  void connect();

  void uploadFile(const std::string &filePath, const std::string &remoteDir , const std::string &filename);

  void downloadFile(const std::filesystem::path, const std::string &remoteDir , const std::string &filename);

  void safeSend(const std::string & message);

  void updateProgress(std::string filename,int TotalBytes,int TransedBytes);

  std::string safeRecv();

  ~FtpClient();

private:
  Socket controlSocket_;
  InetAddress serverAddr_;

  std::unordered_set<std::string> transList;

  std::unordered_map<std::string,float> transProgressMap;
};