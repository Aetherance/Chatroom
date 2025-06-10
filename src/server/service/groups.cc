#include"ChatServer.h"
#include"ServiceHandler.h"
#include"logger.h"
#include"responsecode.h"
#include<jsoncpp/json/json.h>

extern std::unordered_map<std::string,net::TcpConnectionPtr> userHashConn;

void ServiceHandler::onCreateGroup(const net::TcpConnectionPtr & conn,Message msgProto) {
  std::string creator = msgProto.from();
  std::string group = msgProto.to();

  Message msg;
  if(isGroupExist(group)) {
    msg.set_isservice(true);
    msg.set_text(GROUP_EXIST);
    msg.set_from(group);
    LOG_INFO(creator + " 群名已占用 " + group);
  } else {
    chatServer_->redis_.sadd(chatServer_->allGroupSet,{group});
    chatServer_->redis_.sadd(groupSet + creator,{group});
    chatServer_->redis_.sadd(chatServer_->groupMembers + group,{creator});
    LOG_INFO(creator + " 创建了群 " + group);
    for(int i = 0;i<msgProto.args_size();i++) {
      chatServer_->redis_.sadd(groupSet + msgProto.args(i),{group});
      chatServer_->redis_.sadd(chatServer_->groupMembers + group,{msgProto.args(i)});
    }
    chatServer_->redis_.hset(chatServer_->groupHashOwner,group,creator);
    chatServer_->redis_.sync_commit();
    
    msg.set_isservice(true);
    msg.set_text(CREATE_GROUP);
    msg.set_from(group);
  }

  if(chatServer_->isUserOnline(creator)) {
    chatServer_->sendMsgToUser(msg.SerializeAsString(),userHashConn[creator]);
  } else {
    chatServer_->onOfflineMsg(creator,msg.SerializeAsString());
  }

  for(int i = 0;i<msgProto.args_size();i++) {
    chatServer_->sendOrSave(msgProto.args(i),msg.SerializeAsString());
  }
}

bool ServiceHandler::isGroupExist(const std::string & group) {
  auto future = chatServer_->redis_.sismember(chatServer_->allGroupSet,group);
  chatServer_->redis_.sync_commit();
  auto reply = future.get();

  return reply.as_integer();
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