#ifndef CLIENT_H
#define CLIENT_H

// 服务器IP地址和段口号
#define SERVER_PORT 8080

#include"Socket.h"
#include<vector>

struct Friend;
struct Group;

/* 好友列表 */
extern std::vector<Friend>FriendList;

/* 群聊列表 */
extern std::vector<Group> GroupList;

class UserClient
{
public:
  UserClient(const std::string & ip);
  ~UserClient() { }

  void Connect();

  int SendRegister1(const std::string & email);
  int SendRegister2(const std::string email,const std::string & user_name,const std::string& passwd,const std::string & code);
  
  int RequestLogin(const std::string & email,const std::string & passwd);
  
  bool isConnected() { return isConnected_; }

  void setHasLogin(bool has) { hasLogin_ = has; }

  bool hasLogin() const { return hasLogin_; }
private:
  void Send(std::string msg);
  int Recv();

  std::string ConstructRegister1(const std::string email);

  std::string ConstructRegister2(const std::string email,
              const std::string user_name,const std::string passwd,const std::string code);

  std::string ConstructLogin(const std::string & email,const std::string & passwd);


  void SendLogin(const std::string & email,const std::string & passwd);

  bool isConnected_;

  ilib::net::Socket sock_;

  std::string email_;

  bool hasLogin_ = false;

  const std::string ip_;
};

#endif