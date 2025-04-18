#include"UserClient.h"
#include"InetAddress.h"
#include"jsoncpp/json/json.h"
#include"ResponseCode.h"
#include"logger.h"

void UserClient::Connect() {
  sockaddr_in peerAddr = ilib::net::InetAddress(SERVER_IP,SERVER_PORT).getSockAddr();
  
  int stat = connect(sock_.fd(),reinterpret_cast<sockaddr*>(&peerAddr),sizeof(peerAddr));
  if(stat < 0) {
    LOG_ERROR("UserClient::Connect() : connect failed!");
  }
}

UserClient::UserClient()
  : sock_(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) 
{}

void UserClient::Send(std::string msg) {
  ::send(sock_.fd(),msg.data(),msg.size(),0);
}

std::string UserClient::Recv() {
  int recvCode = -1;
  ::recv(sock_.fd(),&recvCode,sizeof(int),0);
  if(recvCode == -1) {
    LOG_ERROR("UserClient::Recv() : No response received");
  }  
}

std::string UserClient::ConstructBindMessage(const std::string &email) {
  Json::Value BindRequestMessage;
  BindRequestMessage["action"] = BIND_REQUEST;
  BindRequestMessage["email"] = email;

  Json::StreamWriterBuilder writer;
  std::string BindMessage = Json::writeString(writer,BindRequestMessage);

  return BindMessage;
}

void UserClient::SendBindRequest(const std::string& email) {
  std::string BindMessage = ConstructBindMessage(email);
  Send(BindMessage);
}

void UserClient::RequestRegister(const std::string & email) {
  SendBindRequest(email);
}

