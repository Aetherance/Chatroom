#ifndef SERVICE_HANDLER_H
#define SERVICE_HANDLER_H

using namespace ilib;

class FtpServer;
class ChatServer;
class ServiceHandler
{
public:
  ServiceHandler(ChatServer * server);
  ~ServiceHandler() {}

  void onAddFriend(const net::TcpConnectionPtr & conn,Message msgProto);

  void onDeleteFriend(const net::TcpConnectionPtr & conn,Message msgProto);
  
  void onVerifyFriend(const net::TcpConnectionPtr & conn,Message msgProto);
 
  void onBlockFriend(const net::TcpConnectionPtr & conn,Message msgProto);
 
  void onCreateGroup(const net::TcpConnectionPtr & conn,Message msgProto);

  void onAddGroup(const net::TcpConnectionPtr & conn,Message msgProto);
 
  void onVerifyGroup(const net::TcpConnectionPtr & conn,Message msgProto);

  void onQuitGroup(const net::TcpConnectionPtr & conn,Message msgProto);

  void onBreakGroup(const net::TcpConnectionPtr & conn,Message msgProto);

  void onRmGroupMember(const net::TcpConnectionPtr & conn,Message msgProto);

  void onUnBlock(const net::TcpConnectionPtr & conn,Message msgProto);

  void onPullFriendList(const net::TcpConnectionPtr & conn,Message msgProto);

  void FriendBeOnline(const net::TcpConnectionPtr & conn);

  void FriendBeOffline(const net::TcpConnectionPtr & conn);

  void onPullGroupList(const net::TcpConnectionPtr & conn,Message msgProto);

  std::string getGroupOwner(const std::string & group) const;

  void onCancel(const net::TcpConnectionPtr & conn,Message msgProto);

  void onPullGroupMembers(const net::TcpConnectionPtr & conn,Message msgProto);

  bool isUserGroupOwner(const std::string useremail,const std::string & group);

  bool isUserGroupOP(const std::string & useremail,const std::string & group);

  void onSetOP(const net::TcpConnectionPtr & conn,Message msgProto);

  void onDeOP(const net::TcpConnectionPtr & conn,Message msgProto);

  bool isUserGroupMember(const std::string & user,const std::string & group);

  bool isUserBlocked(const std::string & user1,const std::string & user2);

  void onUploadFile(const net::TcpConnectionPtr & conn,Message msgProto);
private:
  /* redis 好友与群聊存储 */
  /* 用户的 好友列表前缀 */
  std::string friendSet = "friendSet:";
  /* 用户的 群聊列表前缀 */
  std::string groupSet = "groupSet:";
  /* 用户的 屏蔽好友列表前缀 */
  std::string blockedFriendSet = "blockedSet:";

  /* 群 的管理员列表 */
  std::string groupOpSet = "groupOpSet:";

  ChatServer * chatServer_;

  FtpServer * ftpServer_;
};

#endif