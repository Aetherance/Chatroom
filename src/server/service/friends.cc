#include"ChatServer.h"
#include"ServiceHandler.h"
#include"logger.h"
#include"responsecode.h"

extern std::unordered_map<std::string,net::TcpConnectionPtr> userHashConn;

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
  chatServer_->redis_.srem(blockedFriendSet + msgProto.to(),{msgProto.from()});
  chatServer_->redis_.srem(blockedFriendSet + msgProto.from(),{msgProto.to()});

  chatServer_->redis_.sync_commit();

  Message msg;
  msg.set_isservice(true);
  msg.set_from(conn->user_email());
  msg.set_text(DEL_FRIEND_SUCCESS);

  chatServer_->sendOrSave(msgProto.to(),msg.SerializeAsString());
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
  msgProto.set_text(PULL_FRIEND_LIST);
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

bool ServiceHandler::isUserBlocked(const std::string & user,const std::string & one) {
  auto future = chatServer_->redis_.sismember(blockedFriendSet + one,user);
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  return reply.as_integer();
}

void ServiceHandler::onBlockFriend(const net::TcpConnectionPtr & conn,Message msgProto) {
  chatServer_->redis_.sadd(blockedFriendSet + msgProto.from(),{msgProto.to()});
  chatServer_->redis_.sync_commit();
  LOG_INFO("User " + msgProto.to() + " is blocked by " + msgProto.from());
}

void ServiceHandler::onUnBlock(const net::TcpConnectionPtr & conn,Message msgProto) {
  chatServer_->redis_.srem(blockedFriendSet + msgProto.from(),{msgProto.to()});
  chatServer_->redis_.sync_commit();
  LOG_INFO("User " + msgProto.from() + " unblocked " + msgProto.to());
}