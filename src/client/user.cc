#include"user.h"
#include"InetAddress.h"
#include<jsoncpp/json/json.h>
#include"responsecode.h"
#include"logger.h"

void UserClient::Connect() {
  sockaddr_in peerAddr = ilib::net::InetAddress(SERVER_IP,SERVER_PORT).getSockAddr();
  
  int stat = connect(sock_.fd(),reinterpret_cast<sockaddr*>(&peerAddr),sizeof(peerAddr));
  if(stat < 0) {
    LOG_ERROR("UserClient::Connect() : connect failed!");
  } else {
    isConnected_ = true;
  }
}

UserClient::UserClient()
  : sock_(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) ,
    isConnected_(false)
{}

/* 发送请求 */
void UserClient::Send(std::string msg) {
  ::send(sock_.fd(),msg.data(),msg.size(),0);
}

/* 接收服务端响应码 */
/* 服务端只会回复响应码 简化客户端处理流程 */
int UserClient::Recv() {
  int recvCode = -1;
  ::recv(sock_.fd(),&recvCode,sizeof(int),0);
  if(recvCode == -1) {
    LOG_ERROR("UserClient::Recv() : No response received");
  }
  return recvCode;
}

int UserClient::RequestLogin(const std::string& email,const std::string & passwd) {
  SendLogin(email,passwd);
  int recv = -1;
  recv = Recv();
  std::cout<<recv<<"\n";
  return recv;
}

/* REGISTER 1 */
int UserClient::SendRegister1(const std::string& email) {
  std::string Register1Msg = ConstructRegister1(email);
  Send(Register1Msg);
  return Recv();
}

std::string UserClient::ConstructRegister1(const std::string email) {
  Json::Value Register1RequestMessage;
  Register1RequestMessage["action"] = REGISTER1;
  Register1RequestMessage["email"] = email;

  Json::StreamWriterBuilder writer;
  std::string Register1Msg = Json::writeString(writer,Register1RequestMessage);

  return Register1Msg;
}

/* REGISTER 2 */
int UserClient::SendRegister2(const std::string email,const std::string & user_name,const std::string & passwd,const std::string &code) {
  std::string Register2Msg = ConstructRegister2(email,user_name,passwd,code);
  Send(Register2Msg);
  return Recv();
}

std::string UserClient::ConstructRegister2(const std::string email,const std::string user_name,const std::string passwd,const std::string code) {
  /* 构造用户信息 */
  Json::Value UserInfo;
  UserInfo["email"] = email;
  UserInfo["username"] = user_name;
  UserInfo["passwd"] = passwd;

  Json::Value Register2RequestMessage;
  Register2RequestMessage["action"] = REGISTER2;
  Register2RequestMessage["userInfo"] = UserInfo;
  Register2RequestMessage["code"] = code;

  Json::StreamWriterBuilder writer;
  std::string Register2Msg = Json::writeString(writer,Register2RequestMessage);

  return Register2Msg;
}

/* LOGIN */
void UserClient::SendLogin(const std::string & email,const std::string & passwd) {
  std::string LogMsg = ConstructLogin(email,passwd);
  Send(LogMsg);
}

std::string UserClient::ConstructLogin(const std::string & email,const std::string & passwd) {
  Json::Value LoginRequestMessage;
  LoginRequestMessage["action"] = LOGIN;
  LoginRequestMessage["email"] = email;
  LoginRequestMessage["passwd"] = passwd;

  Json::StreamWriterBuilder writer;
  std::string LogMsg = Json::writeString(writer,LoginRequestMessage);
  return LogMsg;
}