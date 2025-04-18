#include"UserServer.h"
#include"logger.h"
#include<cpp_redis/cpp_redis>
#include"ResponseCodeServer.h"
#include<jsoncpp/json/json.h>

using namespace net;

#define USER_SERVER_REACTOR_NUM_ 4


UserServer::UserServer()
          : addr_(SERVER_PORT),
            server_(&loop_,addr_)
{
  /* 设置TcpServer的回调函数 */
  server_.setThreadNum(USER_SERVER_REACTOR_NUM_);
  server_.setConnectionCallback([this](const TcpConnectionPtr & conn){ onConnection(conn); });
  server_.setMessageCallback([this](const TcpConnectionPtr & conn,Buffer * buff,Timestamp time){ onMessage(conn,buff,time); });

  /* 设置redis */
  redis_.connect("127.0.0.1",6379);
}

void UserServer::onConnection(const net::TcpConnectionPtr & conn) {
  
}

void UserServer::onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time) {
  std::string logInfo = "receive a message from " + conn->peerAddress().toIpPort();
  LOG_INFO(logInfo);
  std::string Message(buff->peek(),buff->readableBytes());
  
  /* 处理消息 */
  parseMessage(Message,conn);
  
  /* 打印日志 */
  buff->retrieveAll();
  LOG_CLIENT_INFO("sent\n" + Message, conn->fd());
}

/* 启动服务器 */
void UserServer::run() {
  LOG_INFO("The server is starting up...");
  server_.start();
  
  std::string success = "The server is running on " + addr_.toIpPort() + " now !";
  LOG_INFO_SUCCESS(success);
  
  loop_.loop();
}

/* 注册: 第一步 */
void UserServer::Register1(const std::string & email,const TcpConnectionPtr & conn) {
  /* 检查邮箱是不是已经存在了 */
  auto future = redis_.hexists(RedisUserInfosHashEmail_,email);
  redis_.sync_commit();
  auto reply = future.get();
  if(reply.as_integer()) {
    LOG_CLIENT_WARN("Register1: Email already exists!",conn->fd());
    SendResponseCode(EMAIL_ALREADY_REGISTERED,conn->fd());
    return;
  }

  std::string VerifiCode = GenerateVerifiCode();
  
  redis_.hset(RedisEmailCodeHash_,email,VerifiCode);
  redis_.sync_commit();
  SendCodeToEmall(VerifiCode,email);
  loop_.runAfter(5 * 60,[this,&email]{ redis_.hdel(RedisEmailCodeHash_,{email}),redis_.sync_commit(); });
  SendResponseCode(USER_OK,conn->fd());
}

/* 注册: 第二步 */ 
void UserServer::Register2(const std::string & userInfo,const std::string & email,const std::string & code,const TcpConnectionPtr & conn) {
  auto future_code = redis_.hget(RedisEmailCodeHash_,email);
  redis_.sync_commit();
  auto reply = future_code.get();
  if(reply.as_string() != code) {
    SendResponseCode(VERIFICATION_CODE_INVALID,conn->fd());
    return;
  }
  redis_.hset(RedisUserInfosHashEmail_,email,userInfo);
  redis_.sync_commit();
  SendResponseCode(USER_OK,conn->fd());
}

std::string fromJsonObjToStr(Json::Value & root) {
  Json::StreamWriterBuilder writer;
  return Json::writeString(writer,root);
}

void UserServer::parseMessage(const std::string & MessageStr,const TcpConnectionPtr & conn) {
  Json::Value message;

  /* 构造Json对象 */
  Json::CharReaderBuilder readerBuilder;
  std::istringstream MessageStream(MessageStr);
  Json::parseFromStream(readerBuilder,MessageStream,&message,nullptr);

  std::string action = message["action"].asString();

  if(action == REGISTER1) {
    Register1(message["email"].asString(),conn);
  } else if(action == REGISTER2) {
    Register2(fromJsonObjToStr(message["userInfo"]),message["userInfo"]["email"].asString(),message["code"].asString(),conn);
  }
}