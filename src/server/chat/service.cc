#include"ChatServer.h"
#include"ServiceHandler.h"
#include"responsecode.h"
#include"msg.pb.h"
#include"logger.h"
#include<algorithm>
#include<jsoncpp/json/json.h>
#include<sstream>

using namespace net;

extern std::unordered_map<std::string,net::TcpConnectionPtr> userHashConn;

ServiceHandler::ServiceHandler(ChatServer * server) : chatServer_(server) {
  server->serviceCallBacks_[ADD_FRIEND] = std::bind(&ServiceHandler::onAddFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[DEL_FRIEND] = std::bind(&ServiceHandler::onDeleteFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[VER_FRIEND] = std::bind(&ServiceHandler::onVerifyFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[BLOCK] = std::bind(&ServiceHandler::onBlockFriend,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[CREATE_GROUP] = std::bind(&ServiceHandler::onCreateGroup,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[ADD_GROUP] = std::bind(&ServiceHandler::onAddGroup,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[QUIT_GROUP] = std::bind(&ServiceHandler::onQuitGroup,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[BREAK_GROUP] = std::bind(&ServiceHandler::onBreakGroup,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[RM_GROUP_MEM] = std::bind(&ServiceHandler::onRmGroupMember,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[PULL_FRIEND_LIST] = std::bind(&ServiceHandler::onPullFriendList,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[PULL_GROUP_LIST] = std::bind(&ServiceHandler::onPullGroupList,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[VERI_GROUP] = std::bind(&ServiceHandler::onVerifyGroup,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[CANCEL] = std::bind(&ServiceHandler::onCancel,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[PULL_GROUP_MEMBERS] = std::bind(&ServiceHandler::onPullGroupMembers,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[SET_OP] = std::bind(&ServiceHandler::onSetOP,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[DE_OP] = std::bind(&ServiceHandler::onDeOP,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[RM_GROUP_MEM] = std::bind(&ServiceHandler::onRmGroupMember,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[UNBLOCK] = std::bind(&ServiceHandler::onUnBlock,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[UPLOAD_FILE] = std::bind(&ServiceHandler::onUploadFile,this,std::placeholders::_1,std::placeholders::_2);
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

void ServiceHandler::onBlockFriend(const net::TcpConnectionPtr & conn,Message msgProto) {
  chatServer_->redis_.sadd(blockedFriendSet + msgProto.from(),{msgProto.to()});
  chatServer_->redis_.sync_commit();
  LOG_INFO("User " + msgProto.to() + " is blocked by " + msgProto.from());
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
    auto future = chatServer_->redis_.smembers(groupOpSet + msgProto.to());
    chatServer_->redis_.sync_commit();
    auto reply = future.get();
    
    for(auto &entry : reply.as_array()) {
      chatServer_->sendOrSave(entry.as_string(),msgProto.SerializeAsString());
    }
    
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
  std::string requestor = msgProto.from();
  std::string group = msgProto.to();
  std::string group_owner = getGroupOwner(group);
  
  if(group_owner != requestor) {
    chatServer_->redis_.srem(groupSet + requestor,{group});
    chatServer_->redis_.srem(chatServer_->groupMembers + group,{requestor});
  }

  Message response_owner;
  response_owner.set_from(requestor);
  response_owner.set_text(MEMBER_QUIT_GROUP);
  response_owner.set_isservice(true);
  response_owner.set_to(group);

  Message response_requestor;
  response_owner.set_from(group);
  response_owner.set_text(QUIT_GROUP_BACK);
  response_owner.set_isservice(true);

  if(group_owner == requestor) {
    onBreakGroup(conn,msgProto);
    LOG_INFO("Group " + group + " 's owner want to quit, the group break");
  } else {
    response_requestor.set_to(QUIT_GROUP_SUCCESS);
  }

  if(response_requestor.to() == QUIT_GROUP_SUCCESS) {
    chatServer_->sendOrSave(group_owner,response_owner.SerializeAsString());
    LOG_INFO(requestor + " quited " + group);
  }
  chatServer_->sendOrSave(requestor,response_requestor.SerializeAsString());
}

void ServiceHandler::onBreakGroup(const net::TcpConnectionPtr & conn,Message msgProto) {
  std::string requestor = msgProto.from();
  std::string group = msgProto.to();
  std::string group_owner = getGroupOwner(group);
  if(group_owner == requestor) {
    chatServer_->redis_.srem(chatServer_->allGroupSet,{group});
    auto future = chatServer_->redis_.smembers(chatServer_->groupMembers + group);
    chatServer_->redis_.sync_commit();
    auto reply = future.get();
    for(auto & entry : reply.as_array()) {
      Message response;
      response.set_from(group);
      response.set_text(GROUP_BROKEN);
      response.set_isservice(true);
      chatServer_->redis_.srem(groupSet + entry.as_string(),{group});
      chatServer_->sendOrSave(entry.as_string(),response.SerializeAsString());
    }
    chatServer_->redis_.srem(chatServer_->allGroupSet,{group});
    chatServer_->redis_.hdel(chatServer_->groupHashOwner,{group});
    chatServer_->redis_.del({chatServer_->groupMembers + group});
    chatServer_->redis_.sync_commit();
    LOG_INFO("Group " + group + " was broken!");
  }
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

void ServiceHandler::onPullGroupList(const net::TcpConnectionPtr & conn,Message msgProto) {
  auto future = chatServer_->redis_.smembers(groupSet + msgProto.from());
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  auto replyArr = reply.as_array();

  for(auto & entry : replyArr) {
    msgProto.add_args(entry.as_string());
  }

  msgProto.set_to(msgProto.from());
  msgProto.set_text(PULL_GROUP_LIST);
  msgProto.set_isservice(true);
  std::string resp = msgProto.SerializeAsString();

  chatServer_->sendMsgToUser(resp,conn);
  LOG_INFO_SUCCESS(conn->user_email() + ": Pull group list!");
}

std::string ServiceHandler::getGroupOwner(const std::string & group) const {
  auto future = chatServer_->redis_.hget(chatServer_->groupHashOwner,group);
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  if(reply.is_string()) {
    auto owner = reply.as_string();
    return owner;
  } else {
    LOG_FATAL("getGroupOwner: reply is not a string");
    return "";
  }
}

void ServiceHandler::onCancel(const net::TcpConnectionPtr & conn,Message msgProto) {
  std::string user = msgProto.from();
  chatServer_->redis_.srem(chatServer_->allUserset,{user});
  
  auto future_friends = chatServer_->redis_.smembers(friendSet + user);
  chatServer_->redis_.sync_commit();
  auto reply_friends = future_friends.get();
  
  for(auto & entry : reply_friends.as_array()) {
    std::string entryFriend = entry.as_string();
    Message deleteFriendMsg = msgProto;
    deleteFriendMsg.set_to(entry.as_string());
    deleteFriendMsg.set_text(DEL_FRIEND);
    onDeleteFriend(conn,deleteFriendMsg);
  }

  auto future_groups = chatServer_->redis_.smembers(groupSet + user);
  chatServer_->redis_.sync_commit();
  auto reply_groups = future_groups.get();

  for(auto & entry : reply_groups.as_array()) {
    std::string entryGroup = entry.as_string();
    Message quitGroupMsg = msgProto;
    quitGroupMsg.set_to(entryGroup);
    quitGroupMsg.set_text(QUIT_GROUP);
    onQuitGroup(conn,quitGroupMsg); 
  }

  chatServer_->redis_.del({chatServer_->offlineMessages + user});
  chatServer_->redis_.srem(chatServer_->onlineUserSet,{user});
  chatServer_->redis_.hdel("email_userinfos",{user});
  chatServer_->redis_.srem("emailHashUserName",{user});
  chatServer_->redis_.sync_commit();
}

void ServiceHandler::onPullGroupMembers(const net::TcpConnectionPtr & conn,Message msgProto) {
  std::string request_user = conn->user_email();
  std::string request_group = msgProto.from();
  auto future = chatServer_->redis_.smembers(chatServer_->groupMembers + request_group);
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  Message responseMessage;
  responseMessage.set_isservice(true);
  responseMessage.set_text(PULL_GROUP_MEMBERS);
  responseMessage.set_to(request_group);

  std::vector<std::string> userinfos; 
  for(auto & useremail : reply.as_array()) {
    auto future = chatServer_->redis_.hget("emailHashUserName",useremail.as_string());
    chatServer_->redis_.sync_commit();
    auto username = future.get().as_string();
    std::string memberLevel;
    if(isUserGroupOwner(useremail.as_string(),request_group)) {
      memberLevel = GROUP_OWNER;
    } else if(isUserGroupOP(useremail.as_string(),request_group)) {
      memberLevel = GROUP_OP;
    } else {
      memberLevel = GROUP_MEMBER;
    }
    const std::string userinfo = username + " [" + useremail.as_string() + "]" + memberLevel;
    userinfos.push_back(userinfo);
  }

  std::sort(userinfos.begin(),userinfos.end(),[](std::string first,std::string second){ 
    char first_level = first[first.size()-1],second_level = second[second.size()-1];
    return first_level < second_level ? true : false;
  });

  for(auto & userinfo : userinfos) {
    responseMessage.add_args(userinfo);
  }

  chatServer_->sendOrSave(request_user,responseMessage.SerializeAsString());
  LOG_INFO("User " + request_user + " pull group " + request_group + " 's member list.");
}

bool ServiceHandler::isUserGroupOwner(const std::string useremail,const std::string & group) {
  return useremail == getGroupOwner(group);
}

bool ServiceHandler::isUserGroupOP(const std::string & useremail,const std::string & group) {
  auto future = chatServer_->redis_.smembers(groupOpSet + group);
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  for(auto & entry : reply.as_array()) {
    if(useremail == entry.as_string()) {
      return true;
    }
  }

  return false;
}

void ServiceHandler::onSetOP(const net::TcpConnectionPtr & conn,Message msgProto) {
  std::string info = msgProto.to();
  Json::CharReaderBuilder reader;
  Json::Value root;
  std::stringstream stm(info);
  Json::parseFromStream(reader,stm,&root,nullptr);
  std::string user = root["useremail"].asString();
  std::string group = root["group"].asString();

  if( !isUserGroupOP(msgProto.from(),group) && !isUserGroupOwner(msgProto.from(),group)) {
    LOG_WARN("Group " + group + " 's normal user " + msgProto.from() + " is try to use operator command");
    return ;
  }

  if( !isUserGroupMember(user,group)) {
    LOG_WARN("user is not a member of the group !");
    return;
  }

  chatServer_->redis_.sadd(groupOpSet + group,{user});
  chatServer_->redis_.sync_commit();

  LOG_INFO(user + " become an op of " + group);
}

bool ServiceHandler::isUserGroupMember(const std::string & user,const std::string & group) {
  auto future = chatServer_->redis_.smembers(chatServer_->groupMembers + group);
  chatServer_->redis_.sync_commit();
  auto reply = future.get();

  for(auto & entry : reply.as_array()) {
    if(entry.as_string() == user) {
      return true;
    }
  }
  return false;
}

void ServiceHandler::onDeOP(const net::TcpConnectionPtr & conn,Message msgProto) {
  std::string info = msgProto.to();
  Json::CharReaderBuilder reader;
  Json::Value root;
  std::stringstream stm(info);
  Json::parseFromStream(reader,stm,&root,nullptr);

  std::string user = root["useremail"].asString();
  std::string group = root["group"].asString();

  if( !isUserGroupOP(msgProto.from(),group) && !isUserGroupOwner(msgProto.from(),group)) {
    LOG_WARN("Group " + group + " 's normal user " + msgProto.from() + " is try to use operator command");
    return ;
  }

  if( !isUserGroupMember(user,group)) {
    LOG_WARN("user is not a member of the group !");
    return;
  }

  chatServer_->redis_.srem(groupOpSet + group,{user});
  chatServer_->redis_.sync_commit();

  LOG_INFO(user + " become an op of " + group);
}

void ServiceHandler::onRmGroupMember(const net::TcpConnectionPtr & conn,Message msgProto) {
  std::string info = msgProto.to();
  Json::CharReaderBuilder reader;
  Json::Value root;
  std::stringstream stm(info);
  Json::parseFromStream(reader,stm,&root,nullptr);

  std::string user = root["useremail"].asString();
  std::string group = root["group"].asString();

  if( !isUserGroupOP(msgProto.from(),group) && !isUserGroupOwner(msgProto.from(),group)) {
    LOG_WARN("Group " + group + " 's normal user " + msgProto.from() + " is try to use operator command");
    return ;
  }

  if( !isUserGroupMember(user,group)) {
    LOG_WARN("user is not a member of the group !");
    return;
  }

  if(isUserGroupOwner(user,group)) {
    return ;
  }

  if(isUserGroupOP(user,group) && isUserGroupOwner(msgProto.from(),group)) {
    chatServer_->redis_.srem(groupOpSet + group,{user});
    chatServer_->redis_.sync_commit();
  }
  
  Message ServiceMsg;
  ServiceMsg.set_text(QUIT_GROUP);
  ServiceMsg.set_from(user);
  ServiceMsg.set_to(group);
  ServiceMsg.set_isservice(true);

  onQuitGroup(conn,ServiceMsg);

  LOG_INFO("User " + user + " was remove from " + group);
}

bool ServiceHandler::isUserBlocked(const std::string & user,const std::string & one) {
  auto future = chatServer_->redis_.sismember(blockedFriendSet + one,user);
  chatServer_->redis_.sync_commit();
  auto reply = future.get();
  return reply.as_integer();
}

void ServiceHandler::onUnBlock(const net::TcpConnectionPtr & conn,Message msgProto) {
  chatServer_->redis_.srem(blockedFriendSet + msgProto.from(),{msgProto.to()});
  chatServer_->redis_.sync_commit();
  LOG_INFO("User " + msgProto.from() + " unblocked " + msgProto.to());
}

void ServiceHandler::onUploadFile(const net::TcpConnectionPtr & conn,Message msgProto) {
  const std::string to_user = msgProto.args(0);
  chatServer_->sendOrSave(to_user,msgProto.SerializeAsString());
  LOG_INFO("User " + msgProto.from() + " upload file to " + to_user);
}