#ifndef MSG_H
#define MSG_H

#include<string>
#include"arpa/inet.h"
#include"InetAddress.h"
#include"Buffer.h"
#include"responsecode.h"

struct Friend {
  std::string email;
  std::string username;
};

struct Group {
  std::string groupId;
  std::string groupname;
};

/* 好友列表 */
inline std::vector<Friend> FriendList;

/* 群聊列表 */
inline std::vector<Group> GroupList;

class MsgClient
{
public:
  MsgClient();
  
  ~MsgClient();
  
  void connect();
  
  void safeSend(const std::string & msg);

  inline void sendMsgPeer(const std::string & msg) { sendMsgTo(msgPeerEmail_,msg); }
  
  void updatePeer(const std::string &newPeerEmail,const std::string & newPeerUsername);
  
  void recvMsgLoop();
  
  void onMessage();

  void setEmail(const std::string & email) { email_ = email; }

  std::string peerUsername() { return msgPeerUsername_; }
  
  std::string peerEmail() { return msgPeerEmail_; }

  void SerializeSend(const std::string action, const std::string & Requestor,const std::string & obj,const std::vector<std::string>& args = {});

  void addFriend(const std::string & requestor,const std::string & obj);

  void deleteFriend(const std::string & requestor,const std::string & obj);

  void verifyFriend(const std::string & requestor,const std::string & obj);

  void blackoutFriend(const std::string & requestor,const std::string & obj);

  void blockFriend(const std::string & requestor,const std::string & obj);

  void addGroup(const std::string & requestor,const std::string & obj);

  void quitGroup(const std::string & requestor,const std::string & obj);

  void createGroup() {

  }

  void rmGroupMember() {

  }

  void breakGroup() {

  }

  // void pullFriendList();
  
  // void pushFriendList();

private:
  void sendMsgTo(const std::string & who,const std::string & msg);
  void echoMsg(const std::string & sourceMsg,std::string & echoMsg);

  /* 用户信息 */
  std::string username_;
  std::string email_;

  /* 当前聊天对象信息 */
  /* 群号或邮箱 */
  std::string msgPeerEmail_;
  std::string msgPeerUsername_;
  
  ilib::net::InetAddress chatServerAddr_;
  int chatServerfd_;

  ilib::net::Buffer recvBuff_;
};

#endif