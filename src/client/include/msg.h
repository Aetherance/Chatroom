#ifndef MSG_H
#define MSG_H

#include<string>
#include"arpa/inet.h"
#include"InetAddress.h"
#include"Buffer.h"
#include"responsecode.h"
#include"msg.pb.h"
#include"EventLoop.h"

struct Friend {
  std::string email;
  std::string username;
  bool isOnline;
};

struct Group {
  std::string groupname;
};

struct messageinfo {
  std::string from;
  std::string text;
  int64_t timestamp;
};

struct GroupApplication {
  std::string user;
  std::string group;
};

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

  void parseMsg(std::string msg);

  void setEmail(const std::string & email) { LocalEmail_ = email; }

  std::string peerUsername() { return msgPeerUsername_; }
  
  std::string peerEmail() { return msgPeerEmail_; }

  std::string LocalUsername() const { return LocalUsername_; }

  std::string LocalEmail() const { return LocalEmail_; }

  void SerializeSend(const std::string action, const std::string & Requestor,const std::string & obj,const std::vector<std::string>& args = {});

  void addFriend(const std::string & requestor,const std::string & obj);

  void deleteFriend(const std::string & requestor,const std::string & obj);

  void verifyFriend(const std::string & requestor,const std::string & obj);

  void blockFriend(const std::string & requestor,const std::string & obj);

  void addGroup(const std::string & requestor,const std::string & obj);

  void quitGroup(const std::string & requestor,const std::string & obj);

  void createGroup(const std::string & creator, const std::string & group,const std::vector<std::string> & members);

  void verifyGroup(const std::string & who, const std::string & group);

  void rmGroupMember(const std::string & who,const std::string & group);

  void breakGroup(const std::string & requestor,const std::string & obj);

  void unBlock(const std::string & requestor,const std::string & obj);

  void pullFriendList(bool isRecv = false,Message msg = {});
  
  void doService(Message msg);

  void doAddFriendBack(const Message & msg);

  void doAddFriend(const Message & msg);

  void doUpdateFriendState(const Message & msg,bool isOnline);

  void doDeleteFriend(const Message & msg);

  void doCreateGroup(const Message & msg);

  void pullGroupList(bool isRecv = false,Message msg = {});

  void doAddGroupBack(const Message & msg);

  void doAddGroup(const Message & msg);

  void doVeriGroup(const Message & msg);

  void doGroupMemberQuit(const Message & msg);

  void doQuitGroup(const Message & msg);

  bool isPeerGroup() const;

  void setIsPeerGroup(bool is) { isPeerGroup_ = is; }

  void CancelAccount(const std::string & account);

  void ExitLoop() { loop_->quit(); }

  void enMapYouMessage(Message msgProto);

  void pullGroupMembers(bool isRecv = false,std::string = {},Message msg = {});

  void setOP(const std::string & user,const std::string & group);

  void deOP(const std::string & user,const std::string & group);

  void doBlockedMessage(Message);

  void doRecvFile(Message message);

  void doHeartBeat(Message message);

  void doGroupExist(Message message);

  std::vector<std::string>& getGroupMembers(std::string group) { return groupMembers[group]; }
private:
  void sendMsgTo(const std::string & who,const std::string & msg);

  /* 用户信息 */
  std::string LocalUsername_;
  std::string LocalEmail_;

  /* 当前聊天对象信息 */
  /* 群号或邮箱 */
  std::string msgPeerEmail_;
  std::string msgPeerUsername_;
  
  ilib::net::InetAddress chatServerAddr_;
  int chatServerfd_;

  ilib::net::Buffer recvBuff_;

  bool isPeerGroup_;

  ilib::net::EventLoop * loop_;

  std::unordered_map<std::string,std::vector<std::string>> groupMembers;
};

#endif