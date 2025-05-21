#ifndef SERVICE_HANDLER_H
#define SERVICE_HANDLER_H

using namespace ilib;

class ChatServer;
class ServiceHandler
{
public:
  ServiceHandler(ChatServer * server);
  ~ServiceHandler() {}

  void onAddFriend(const net::TcpConnectionPtr & conn,Message msgProto);

  void onDeleteFriend(const net::TcpConnectionPtr & conn,Message msgProto);
  
  void onVerifyFriend(const net::TcpConnectionPtr & conn,Message msgProto);
 
  void onBlackoutFriend(const net::TcpConnectionPtr & conn,Message msgProto);
 
  void onBlockFriend(const net::TcpConnectionPtr & conn,Message msgProto);
 
  void onCreateGroup(const net::TcpConnectionPtr & conn,Message msgProto);

  void onAddGroup(const net::TcpConnectionPtr & conn,Message msgProto);
 
  void onVerifyGroup(const net::TcpConnectionPtr & conn,Message msgProto);

  void onQuitGroup(const net::TcpConnectionPtr & conn,Message msgProto);

  void onBreakGroup(const net::TcpConnectionPtr & conn,Message msgProto);

  void onRmGroupMember(const net::TcpConnectionPtr & conn,Message msgProto);

  void onPullFriendList(const net::TcpConnectionPtr & conn,Message msgProto);

  void FriendBeOnline(const net::TcpConnectionPtr & conn);

  void FriendBeOffline(const net::TcpConnectionPtr & conn);

  void onPullGroupList(const net::TcpConnectionPtr & conn,Message msgProto);

  std::string getGroupOwner(const std::string & group) const; 

private:
  /* redis 好友与群聊存储 */
  /* 好友列表前缀 */
  std::string friendSet = "friendSet:";
  /* 群聊列表前缀 */
  std::string groupSet = "groupSet:";
  /* 屏蔽好友列表前缀 */
  std::string blockedFriendSet = "blockedSet:";

  ChatServer * chatServer_;
};

#endif