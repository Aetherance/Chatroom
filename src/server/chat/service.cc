#include"ChatServer.h"
#include"ServiceHandler.h"
#include"responsecode.h"
#include"msg.pb.h"

using namespace net;

ServiceHandler::ServiceHandler(ChatServer * server)
{
  server->serviceCallBacks_[ADD_FRIEND] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[DEL_FRIEND] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[VER_FRIEND] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[BLACK_OUT] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[BLOCK] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[CREATE_GROUP] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[ADD_GROUP] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[QUIT_GROUP] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[BREAK_GROUP] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[RM_GROUP_MEM] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
}

void ServiceHandler::onAddFriend(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onDeleteFriend(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onVerifyFriend(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onBlackoutFriend(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onBlockFriend(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onCreateGroup(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onAddGroup(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onQuitGroup(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onBreakGroup(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onRmGroupMember(const net::TcpConnectionPtr & conn,Message msgProto) {
  
}