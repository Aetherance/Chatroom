#include"TcpServer.h"
#include<cpp_redis/cpp_redis>
#include<memory>
using namespace ilib;

#define SERVER_PORT 8080

class UserServer
{
public:
  UserServer();
  void run();
private:
  /* 网络组件 */
  /* addr_必须先于server声明 */
  net::InetAddress addr_;
  net::TcpServer server_;
  net::EventLoop loop_;
  
  /* 数据库 */
  cpp_redis::client redis_;

  /* 连接回调 */
  void onConnection(const net::TcpConnectionPtr & conn);
  /* 消息回调 */
  void onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time);

  /* 消息解析 */
  void parseMessage(const std::string & Messgae,const net::TcpConnectionPtr & conn);

  /* 注册功能 */
  void onRegister1(const std::string & email,const net::TcpConnectionPtr & conn);
  void onRegister2(const std::string & userInfo,const std::string & email,const std::string & code,const net::TcpConnectionPtr & conn);

  /* 注册功能: 验证码API */
  std::string GenerateVerifiCode();
  void SendCodeToEmall(const std::string &code,const std::string & email);
  
  /* 登录功能 */
  void onLogin(const std::string& email,const std::string& passwd,const net::TcpConnectionPtr & conn);
  
  /* redis email-code 哈希表的名称 */
  inline static const std::string RedisEmailCodeHash_ = "email_code";
  /* redis 用户信息哈希表的名称 */
  inline static const std::string RedisUserInfosHashEmail_ = "email_userinfos";
};