#include"ChatServer.h"
#include"ServiceHandler.h"
#include"responsecode.h"
#include"msg.pb.h"
#include"logger.h"
#include<algorithm>
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
  server->serviceCallBacks_[PULL_DL_LIST] = std::bind(&ServiceHandler::onPullDownloadList,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[PULL_ALL_USERS] = std::bind(&ServiceHandler::onPullAllUser,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[PULL_GROUP_OWNER] = std::bind(&ServiceHandler::onPullGroupOwner,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[PULL_GROUP_OPS] = std::bind(&ServiceHandler::onPullGroupOPs,this,std::placeholders::_1,std::placeholders::_2);
  server->serviceCallBacks_[REJECT] = std::bind(&ServiceHandler::onReject,this,std::placeholders::_1,std::placeholders::_2);
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
  chatServer_->redis_.hdel("emailHashUserName",{user});
  chatServer_->redis_.sync_commit();
}

bool ServiceHandler::isUserGroupOwner(const std::string useremail,const std::string & group) {
  return useremail == getGroupOwner(group);
}

bool ServiceHandler::isUserGroupMember(const std::string & user,const std::string & group) {
  auto future = chatServer_->redis_.smembers(chatServer_->groupMembers + group);
  chatServer_->redis_.sync_commit();
  auto reply = future.get();

  for(auto & entry : reply.as_array()) {
    if(entry.as_string() == user) {
      LOG_INFO(user + " is a member of " + group);
      return true;
    }
  }
  LOG_WARN(user + " is not a member of " + group);
  return false;
}

void ServiceHandler::onUploadFile(const net::TcpConnectionPtr & conn,Message msgProto) {
  const std::string to_user = msgProto.args(0);
  if(isGroupExist(to_user)) {
    msgProto.set_to(to_user);
    chatServer_->onGroupMessage(to_user,msgProto);
  } else {
    chatServer_->sendOrSave(to_user,msgProto.SerializeAsString());
    LOG_INFO("User " + msgProto.from() + " upload file to " + to_user);
  }
}

void ServiceHandler::onPullDownloadList(const net::TcpConnectionPtr & conn,Message msgProto) {
  LOG_INFO_SUCCESS("PULL_DOWNLOAD_LIST");
  auto future = (isGroupExist(msgProto.to()) ? chatServer_->redis_.smembers(redisFileToFromSet_ + msgProto.to()) : chatServer_->redis_.smembers(redisFileToFromSet_ + msgProto.from() + "/" + msgProto.to()));
  chatServer_->redis_.sync_commit();
  auto reply = future.get();

  Message back;
  back.set_text(PULL_DL_LIST);
  back.set_isservice(true);
  for(auto & entry : reply.as_array()) {
    back.add_args(entry.as_string());
  }

  chatServer_->sendOrSave(conn->user_email(),back.SerializeAsString());
}

void ServiceHandler::onPullAllUser(const net::TcpConnectionPtr & conn,Message msgProto) {
  auto future = chatServer_->redis_.smembers(chatServer_->allUserset);
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
    friendInfo = entry.as_string() + "\n" + reply.as_string();
    LOG_INFO(friendInfo);
    msgProto.add_args(friendInfo);
  }
  
  msgProto.set_to(msgProto.from());
  msgProto.set_text(PULL_ALL_USERS);
  msgProto.set_isservice(true);
  std::string resp = msgProto.SerializeAsString();
  
  chatServer_->sendMsgToUser(resp,conn);

  LOG_INFO_SUCCESS(conn->user_email() + ": Pull All User List!");
}

void ServiceHandler::onReject(const net::TcpConnectionPtr & conn,Message msgProto) {
  LOG_INFO(msgProto.from() + " rejected " + msgProto.to());
  std::string request = msgProto.SerializeAsString();

  chatServer_->sendOrSave(msgProto.to(),request);
}