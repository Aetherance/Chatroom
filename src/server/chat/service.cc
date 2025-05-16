#include"ChatServer.h"
#include"ServiceHandler.h"
#include"responsecode.h"
#include"msg.pb.h"
#include"logger.h"

using namespace net;

extern std::unordered_map<std::string,net::TcpConnectionPtr> userHashConn;

ServiceHandler::ServiceHandler(ChatServer * server) : chatServer_(server) {
  server->serviceCallBacks_[ADD_FRIEND] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[DEL_FRIEND] = std::bind(&ServiceHandler::onDeleteFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[VER_FRIEND] = std::bind(&ServiceHandler::onVerifyFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[BLACK_OUT] = std::bind(&ServiceHandler::onBlackoutFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[BLOCK] = std::bind(&ServiceHandler::onBlockFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[CREATE_GROUP] = std::bind(&ServiceHandler::onCreateGroup,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[ADD_GROUP] = std::bind(&ServiceHandler::onAddGroup,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[QUIT_GROUP] = std::bind(&ServiceHandler::onQuitGroup,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[BREAK_GROUP] = std::bind(&ServiceHandler::onBreakGroup,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[RM_GROUP_MEM] = std::bind(&ServiceHandler::onRmGroupMember,this,std::placeholders::_1,std::placeholders::_2);
}

std::string generateGroupNum() {
  std::string Num;
  Num.reserve(6);
  srand((size_t)time(NULL));
  for(int i = 0;i<6;i++) {
      Num.push_back((rand() % 10) + '0');
  }
  return Num;
}

void ServiceHandler::onAddFriend(const net::TcpConnectionPtr & conn,Message msgProto) {
  LOG_INFO(msgProto.from() + "wants to make friend with " + msgProto.to());
  std::string request = msgProto.SerializeAsString();
  chatServer_->sendMsgToUser(request,userHashConn[msgProto.to()]);
}

/* 验证好友(确认添加好友) */
void ServiceHandler::onVerifyFriend(const net::TcpConnectionPtr & conn,Message msgProto) {
  LOG_INFO(msgProto.from() + " and " + msgProto.to() + " became friends.");
  chatServer_->redis_.sadd(friendSet + msgProto.to(),{msgProto.from()});
  chatServer_->redis_.sadd(friendSet + msgProto.from(),{msgProto.to()});
  chatServer_->redis_.sync_commit();
}

void ServiceHandler::onDeleteFriend(const net::TcpConnectionPtr & conn,Message msgProto) {
  LOG_INFO(msgProto.from() + " deleted " + msgProto.to());
  chatServer_->redis_.srem(friendSet + msgProto.to(),{msgProto.from()});
  chatServer_->redis_.srem(friendSet + msgProto.from(),{msgProto.to()});
  chatServer_->redis_.sync_commit();
}

void ServiceHandler::onBlackoutFriend(const net::TcpConnectionPtr & conn,Message msgProto) {
  
}

void ServiceHandler::onBlockFriend(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onCreateGroup(const net::TcpConnectionPtr & conn,Message msgProto) {
  std::string GroupNum = generateGroupNum();
  chatServer_->redis_.sadd(groupSet,{msgProto.to()});
  chatServer_->redis_.sync_commit();
}

void ServiceHandler::onAddGroup(const net::TcpConnectionPtr & conn,Message msgProto) {
  
}

void ServiceHandler::onQuitGroup(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onBreakGroup(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onRmGroupMember(const net::TcpConnectionPtr & conn,Message msgProto) {
  
}