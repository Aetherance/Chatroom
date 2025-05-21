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
  server->serviceCallBacks_[PULL_FRIEND_LIST] = std::bind(&ServiceHandler::onPullFriendList,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[PULL_GROUP_LIST] = std::bind(&ServiceHandler::onPullGroupList,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[VERI_GROUP] = std::bind(&ServiceHandler::onVerifyGroup,this,std::placeholders::_1,std::placeholders::_2);
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
  LOG_INFO(msgProto.from() + " wants to make friend with " + msgProto.to());
  std::string request = msgProto.SerializeAsString();
  
  Message returnMsg;
  returnMsg.set_from(ADDFRIEND_BACK);
  returnMsg.set_to(msgProto.from());
  returnMsg.set_isservice(true);

  if(chatServer_->isUserExist(msgProto.to())) {
    chatServer_->sendOrSave(msgProto.to(),request);
    returnMsg.set_text(ADD_FRIEND_SEND_SUCCESS);
  } else {
    returnMsg.set_text(ADD_FRIEND_SEND_FAILED);
  }
  
  chatServer_->sendMsgToUser(returnMsg.SerializeAsString(),conn);
}

/* 验证好友(确认添加好友) */
void ServiceHandler::onVerifyFriend(const net::TcpConnectionPtr & conn,Message msgProto) {
  LOG_INFO(msgProto.from() + " and " + msgProto.to() + " became friends.");
  chatServer_->redis_.sadd(friendSet + msgProto.to(),{msgProto.from()});
  chatServer_->redis_.sadd(friendSet + msgProto.from(),{msgProto.to()});
  chatServer_->redis_.sync_commit();

  Message fromMsg,toMsg;
  fromMsg.set_to(msgProto.from());
  fromMsg.set_text(VERI_FRIEND_BACK);
  fromMsg.set_from(msgProto.from());
  fromMsg.set_isservice(true);

  toMsg.set_to(msgProto.to());
  toMsg.set_text(VERI_FRIEND_BACK);
  fromMsg.set_from(msgProto.from());
  toMsg.set_isservice(true);

  chatServer_->sendMsgToUser(fromMsg.SerializeAsString(),userHashConn[fromMsg.to()]);
  chatServer_->sendMsgToUser(toMsg.SerializeAsString(),userHashConn[toMsg.to()]);
}

void ServiceHandler::onDeleteFriend(const net::TcpConnectionPtr & conn,Message msgProto) {
  LOG_INFO(msgProto.from() + " deleted " + msgProto.to());
  chatServer_->redis_.srem(friendSet + msgProto.to(),{msgProto.from()});
  chatServer_->redis_.srem(friendSet + msgProto.from(),{msgProto.to()});
  chatServer_->redis_.sync_commit();

  Message msg;
  msg.set_isservice(true);
  msg.set_from(conn->user_email());
  msg.set_text(DEL_FRIEND_SUCCESS);

  chatServer_->sendOrSave(msgProto.to(),msg.SerializeAsString());
}

void ServiceHandler::onBlackoutFriend(const net::TcpConnectionPtr & conn,Message msgProto) {
  
}

void ServiceHandler::onBlockFriend(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onCreateGroup(const net::TcpConnectionPtr & conn,Message msgProto) {
  std::string creator = msgProto.from();
  std::string group = msgProto.to();
  chatServer_->redis_.sadd(chatServer_->allGroupSet,{group});
  chatServer_->redis_.sadd(groupSet + creator,{group});
  chatServer_->redis_.sadd(chatServer_->groupMembers + group,{creator});
  LOG_INFO(creator + "创建了群" + group);
  for(int i = 0;i<msgProto.args_size();i++) {
    chatServer_->redis_.sadd(groupSet + msgProto.args(i),{group});
    chatServer_->redis_.sadd(chatServer_->groupMembers + group,{msgProto.args(i)});
  }
  chatServer_->redis_.hset(chatServer_->groupHashOwner,group,creator);
  chatServer_->redis_.sync_commit();

  Message msg;
  msg.set_isservice(true);
  msg.set_text(CREATE_GROUP);
  msg.set_from(group);

  if(chatServer_->isUserOnline(creator)) {
    chatServer_->sendMsgToUser(msg.SerializeAsString(),userHashConn[creator]);
  } else {
    chatServer_->onOfflineMsg(creator,msg.SerializeAsString());
  }

  for(int i = 0;i<msgProto.args_size();i++) {
    chatServer_->sendOrSave(msgProto.args(i),msg.SerializeAsString());
  }
}

void ServiceHandler::onAddGroup(const net::TcpConnectionPtr & conn,Message msgProto) {
  LOG_INFO(msgProto.from() + " wants to join " + msgProto.to());
  std::string request = msgProto.SerializeAsString();
  
  Message returnMsg;
  returnMsg.set_from(ADDGROUP_BACK);
  returnMsg.set_to(msgProto.from());
  returnMsg.set_isservice(true);

  if(chatServer_->isGroupMessage(msgProto.to())) {
    auto future = chatServer_->redis_.hget(chatServer_->groupHashOwner,msgProto.to());
    chatServer_->redis_.sync_commit();
    auto reply = future.get();
    auto owner = reply.as_string();
  
    chatServer_->sendOrSave(owner,msgProto.SerializeAsString());
    returnMsg.set_text(ADD_GROUP_SEND_SUCCESS);
  } else {
    returnMsg.set_text(ADD_GROUP_SEND_FAILED);
  }
  
  chatServer_->sendOrSave(msgProto.from(),returnMsg.SerializeAsString());
}

void ServiceHandler::onVerifyGroup(const net::TcpConnectionPtr & conn,Message msgProto) {
  chatServer_->redis_.sadd(chatServer_->groupMembers + msgProto.to(),{msgProto.from()});
  chatServer_->redis_.sadd(groupSet + msgProto.from(),{msgProto.to()});

  Message response_new_member;
  response_new_member.set_from(msgProto.to());
  response_new_member.set_text(VERI_GROUP_SUCCESS);
  response_new_member.set_isservice(true);
  response_new_member.set_to(ENTERING_NEW_GROUP);

  Message response_owner;
  response_owner.set_from(msgProto.from());
  response_owner.set_text(VERI_GROUP_SUCCESS);
  response_owner.set_isservice(true);
  response_owner.set_to(VERI_GROUP_SUCCESS);

  chatServer_->sendOrSave(msgProto.from(),response_new_member.SerializeAsString());
  chatServer_->sendOrSave(conn->user_email(),response_owner.SerializeAsString());
}

void ServiceHandler::onQuitGroup(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onBreakGroup(const net::TcpConnectionPtr & conn,Message msgProto) {

}

void ServiceHandler::onRmGroupMember(const net::TcpConnectionPtr & conn,Message msgProto) {
  
}

void ServiceHandler::onPullFriendList(const net::TcpConnectionPtr & conn,Message msgProto) {
  auto future = chatServer_->redis_.smembers(friendSet + msgProto.from());
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  auto replyArr = reply.as_array();

  for(auto & entry : replyArr) {
    std::string friendInfo;
    auto future = chatServer_->redis_.hget(EMAIL_HASH_USERNAME,entry.as_string());

    chatServer_->redis_.sync_commit();
    auto reply = future.get();
    if(!reply.is_string()) {
      continue;
    }
    friendInfo = entry.as_string() + "\n" + (chatServer_->isUserOnline(entry.as_string()) ? "O" : "U") + reply.as_string();
    msgProto.add_args(friendInfo);
  }

  msgProto.set_to(msgProto.from());
  msgProto.set_from(PULL_FRIEND_LIST);
  msgProto.set_isservice(true);
  std::string resp = msgProto.SerializeAsString();

  chatServer_->sendMsgToUser(resp,conn);
  LOG_INFO_SUCCESS(conn->user_email() + ": Pull friend list!");
}

void ServiceHandler::FriendBeOnline(const net::TcpConnectionPtr & conn) {
  Message msg;
  msg.set_from(conn->user_email());
  msg.set_text(FRIEND_BE_ONLINE);
  msg.set_isservice(true);

  auto future = chatServer_->redis_.smembers(friendSet + conn->user_email());
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  auto replyArr = reply.as_array();

  for(auto & entry : replyArr) {
    if(chatServer_->isUserOnline(entry.as_string())) {
      chatServer_->sendMsgToUser(msg.SerializeAsString(),userHashConn[entry.as_string()]);
    }
  }
}

void ServiceHandler::FriendBeOffline(const net::TcpConnectionPtr & conn) {
  Message msg;
  msg.set_from(conn->user_email());
  msg.set_text(FRIEND_BE_OFFLINE);
  msg.set_isservice(true);

  auto future = chatServer_->redis_.smembers(friendSet + conn->user_email());
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  auto replyArr = reply.as_array();

  for(auto & entry : replyArr) {
    if(chatServer_->isUserOnline(entry.as_string())) {
      chatServer_->sendMsgToUser(msg.SerializeAsString(),userHashConn[entry.as_string()]);
    }
  }
}

void ServiceHandler::onPullGroupList(const net::TcpConnectionPtr & conn,Message msgProto) {
  auto future = chatServer_->redis_.smembers(groupSet + msgProto.from());
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  auto replyArr = reply.as_array();

  for(auto & entry : replyArr) {
    msgProto.add_args(entry.as_string());
  }

  msgProto.set_to(msgProto.from());
  msgProto.set_from(PULL_GROUP_LIST);
  msgProto.set_isservice(true);
  std::string resp = msgProto.SerializeAsString();

  chatServer_->sendMsgToUser(resp,conn);
}