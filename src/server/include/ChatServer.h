#ifndef CHATSERVER_H
#define CHATSERVER_H

#include"TcpServer.h"
#include"DBWriterPool.h"
#include"msg.pb.h"
#include"ServiceHandler.h"

using namespace ilib;

class ChatServer
{
friend class ServiceHandler;
public:
  ChatServer();
  void run();
private:
  /* 网络组件 */
  net::InetAddress addr_;  
  net::EventLoop loop_;
  net::TcpServer server_;

  /* 数据库 */
  cpp_redis::client redis_;

  /* 连接回调 */
  void onConnection(const net::TcpConnectionPtr & conn);
  
  /* 消息回调 */
  void onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time);
  
  /* 消息解析 */
  void parseMessage(const std::string & Messgae,const net::TcpConnectionPtr & conn);

  /* conn是用户对应的连接 */
  void sendMsgToUser(const std::string & Msg,const net::TcpConnectionPtr & conn);

  bool isUserExist(const std::string & user_email); 

  bool isUserOnline(const std::string & user_email);

  void onOfflineMsg(const std::string & who,const std::string & msg);

  void offlineMsgConsumer(const net::TcpConnectionPtr & conn);
  
  void onGroupMessage(const std::string & group,const std::string & msg);

  bool isGroupMessage(const std::string & obj);

  using serviceCallback = std::function<void(const net::TcpConnectionPtr & conn,Message msgProto)>;
  
  std::unordered_map<std::string,serviceCallback>serviceCallBacks_;
  
  DBWriterPool DBWriter_;

  ServiceHandler serviceHandler_;

  /* redis userset 哈希表的名称 */
  inline static const std::string allUserset = "allUserSet";
  inline static const std::string onlineUserSet = "onlineUserSet";
  inline static const std::string offlineMessages = "offlineMessages:";
  inline static const std::string allGroupSet = "allGroupSet";
  inline static const std::string groupMembers = "groupMembers:";
  inline static const std::string groupHashOwner = "groupHashOwner";
};

#endif