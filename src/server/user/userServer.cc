#include"UserServer.h"
#include"logger.h"
#include<cpp_redis/cpp_redis>
#include"responsecode.h"
#include<jsoncpp/json/json.h>
#include<map>
#include<sstream>

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
  redis_.connect(REDIS_HOST,REDIS_PORT);
}

UserServer::~UserServer() {
  redis_.del({onlineUserSet});
  redis_.sync_commit();
}

void UserServer::onConnection(const net::TcpConnectionPtr & conn) {}

void UserServer::onMessage(const net::TcpConnectionPtr & conn,net::Buffer* buff,Timestamp time) {
  std::string logInfo = "receive a message from " + conn->peerAddress().toIpPort();
  LOG_INFO(logInfo);
  std::string Message(buff->peek(),buff->readableBytes());
  
  LOG_CLIENT_INFO("sent\n" + Message, conn->fd());
  /* 处理消息 */
  parseMessage(Message,conn);
  
  /* 打印日志 */
  buff->retrieveAll();
}

/* 启动服务器 */
void UserServer::run() {
  server_.start();
  
  std::string success = "The User Server is running on port 8080!";
  LOG_INFO_SUCCESS(success);
  
  loop_.loop();
}

/* 注册: 第一步 */
void UserServer::onRegister1(const std::string & email,const TcpConnectionPtr & conn) {
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
  
  redis_.set("code:" + email,VerifiCode);
  redis_.expire("code:" + email, 60 * 5);
  redis_.sync_commit();
  SendCodeToEmall(VerifiCode,email);
  SendResponseCode(USER_OK,conn->fd());
}

/* 注册: 第二步 */ 
void UserServer::onRegister2(const std::string & userInfo,const std::string & email,const std::string & code,const TcpConnectionPtr & conn) {
  auto future_code = redis_.get("code:" + email);
  redis_.sync_commit();
  auto reply = future_code.get();
  if(reply.as_string() != code) {
    SendResponseCode(VERIFICATION_CODE_INVALID,conn->fd());
    return;
  }
  redis_.hset(RedisUserInfosHashEmail_,email,userInfo);
  redis_.sync_commit();
  LOG_INFO("A user registered! Info:\n" + userInfo);
  
  redis_.sadd(allUserset,{email});
  redis_.sync_commit();

  makeEmailNameHash(userInfo,email);

  SendResponseCode(USER_OK,conn->fd());
}

std::string fromJsonObjToStr(Json::Value & root) {
  Json::StreamWriterBuilder writer;
  return Json::writeString(writer,root);
}

/* 登录 */
void UserServer::onLogin(const std::string& email,const std::string& passwd,const TcpConnectionPtr & conn) {
  auto future_isUserOnline = redis_.sismember(onlineUserSet,email);
  redis_.sync_commit();
  auto reply_isUserOnline = future_isUserOnline.get();

  if(reply_isUserOnline.as_integer()) {
    SendResponseCode(USER_HAVE_LOGIN_ED,conn->fd());
    return;
  }
  
  auto future_userinfo = redis_.hget(RedisUserInfosHashEmail_,email);
  redis_.sync_commit();
  
  auto reply = future_userinfo.get();

  if(reply.is_null()) {
    SendResponseCode(PASSWORD_INCORRECT,conn->fd());
    LOG_WARN("User " + email + " Login failed: account not found!");    
  } else {
    std::string reply_userinfo_str = reply.as_string();
  
    Json::Value userInfoVal;
    Json::CharReaderBuilder readerBuilder;
    std::istringstream Stream(reply_userinfo_str);
    Json::parseFromStream(readerBuilder,Stream,&userInfoVal,nullptr);

    std::string reply_passwd = userInfoVal["passwd"].asString();
    
    if(passwd == reply_passwd) {
      SendResponseCode(USER_OK,conn->fd());
      std::string user_name = userInfoVal["username"].asString();
      LOG_INFO_SUCCESS(user_name + " log in to the server.");
      LOG_INFO_SUCCESS(email + " is on " + conn->peerAddress().toIpPort());
      redis_.sadd(onlineUserSet,{email});
      redis_.sync_commit();
    } else {
      SendResponseCode(PASSWORD_INCORRECT,conn->fd());
      LOG_WARN("User " + email + " Login failed: password incorrect!");
    }
  }
}

void UserServer::parseMessage(const std::string & MessageStr,const TcpConnectionPtr & conn) {
  Json::Value message;

  /* 构造Json对象 */
  Json::CharReaderBuilder readerBuilder;
  std::istringstream MessageStream(MessageStr);
  Json::parseFromStream(readerBuilder,MessageStream,&message,nullptr);

  std::string action = message["action"].asString();

  if(action == REGISTER1) {
    onRegister1(message["email"].asString(),conn);
  } else if(action == REGISTER2) {
    onRegister2(fromJsonObjToStr(message["userInfo"]),message["userInfo"]["email"].asString(),message["code"].asString(),conn);
  } else if(action == LOGIN) {
    onLogin(message["email"].asString(),message["passwd"].asString(),conn);
  } else if(action == TOKEN_LOGIN) {
    onTokenLogin(message["token"].asString(),message["email"].asString(),conn);
  } else if(action ==TOKEN_SET) {
    onSetToken(message["token"].asString(),message["email"].asString(),conn);
  }
}

void UserServer::makeEmailNameHash(const std::string & userInfo,const std::string email) {
  Json::CharReaderBuilder reader;
  Json::Value val;
  std::istringstream str(userInfo);
  Json::parseFromStream(reader,str,&val,nullptr);
  redis_.hset(EMAIL_HASH_USERNAME,email,val["username"].asString());
  redis_.sync_commit();
}

void UserServer::onTokenLogin(const std::string & token,const std::string & email,const TcpConnectionPtr & conn) {
  auto future_isUserOnline = redis_.sismember(onlineUserSet,email);
  redis_.sync_commit();
  auto reply_isUserOnline = future_isUserOnline.get();

  if(reply_isUserOnline.as_integer()) {
    SendResponseCode(USER_HAVE_LOGIN_ED,conn->fd());
    LOG_CLIENT_WARN("User already be online",conn->fd());
    return;
  }
  
  auto future = redis_.get("token:" + email);
  redis_.sync_commit();

  auto reply = future.get();

  if(!reply.is_null() && reply.as_string() == token) {
    SendResponseCode(USER_OK,conn->fd());
    LOG_INFO("PASS");
  } else {
    SendResponseCode(TOKEN_NOT_EXIST,conn->fd());
  }
}

void UserServer::onSetToken(const std::string & token,const std::string & email,const TcpConnectionPtr & conn) {
  redis_.set("token:" + email, {token});
  redis_.expire("token:" + email, 12 * 60 * 60);
  redis_.sync_commit();
}