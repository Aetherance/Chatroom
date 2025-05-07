#include"TcpServer.h"
#include<cpp_redis/cpp_redis>
using namespace ilib;

class ChatServer
{
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

  void sendMsgToUser(const std::string & Msg,const net::TcpConnectionPtr & conn);

  bool isUserOnline(const std::string & user_email);

  void onOfflineMsg(const std::string & who,const std::string & msg);

  /* redis userset 哈希表的名称 */
  inline static const std::string allUserset = "allUserSet";
  inline static const std::string onlineUserSet = "onlineUserSet";
  inline static const std::string offlineUserMsgSet = "offlineUserMsgSet";
};