#include"ChatServer.h"
#include"Buffer.h"
#include<assert.h>
#include"logger.h"
#include"msg.pb.h"
#include"responsecode.h"

extern std::unordered_map<std::string,net::TcpConnectionPtr> userHashConn;

extern std::unordered_set<std::string> m_allUserSet;

using namespace net;

void ChatServer::parseMessage(const std::string & msg_str,const net::TcpConnectionPtr & conn) {
  if(msg_str == HEARTBEAT_MSG) {
      heart_.beat(conn);
      return ;
    }

  Message msg;
  if(!msg.ParseFromString(msg_str)) {
    perror("Protobuf: ChatServer");
    return ;
  }

  if(msg.from() == msg.to() && msg.from() == "SET_CONN_USER") {
    conn->set_user_email(msg.text());
    userHashConn[msg.text()] = conn;
    heart_.addFdHash(conn->fd(),msg.text());
    redis_.sadd(onlineUserSet,{msg.text()});
    offlineMsgConsumer(conn);
    serviceHandler_.FriendBeOnline(conn);
    onlineUserSet_.insert(msg.text());
    return ;
  }

  if(isGroupMessage(msg.to()) && !msg.isservice()) {
    onGroupMessage(msg.to(),msg);
  } else {
    bool isExists = isUserExist(msg.to()) | serviceHandler_.isGroupExist(msg.to());
  
    if(isExists == false && !msg.isservice()) {
      tellBlocked(msg.from(),msg.to());
      LOG_ERROR("parseMessage: User or Group Not Found!");
    }

    if(msg.isservice()) {
      /* 当isservice为真 Message的text字段代表服务的类型 */
      serviceCallBacks_[msg.text()](conn,msg);

    } else if( !msg.isservice() && isExists && msg.from() != msg.to()) {
      LOG_INFO(COLOR_YELLOW + msg.from() + COLOR_RESET + " says" + " to " + COLOR_YELLOW + msg.to() + COLOR_RESET + " : " + msg.text());

      if(serviceHandler_.isUserBlocked(msg.from(),msg.to()) || (! isFriendsOf(msg.to(),msg.from()) && (!isGroupMember(msg.to(),msg.from())))) {
        tellBlocked(msg.from(),msg.to());
        return;
      }
      
      if(isUserOnline(msg.to())) {
        assert(userHashConn[msg.to()]);
        /* to字段存储接收端信息 */
        sendMsgToUser(msg_str,userHashConn[msg.to()]);
      } else {
        /* 用户不在线 逻辑 */
        LOG_INFO("User is not online , message stored!");
        onOfflineMsg(msg.to(),msg_str);
      }

      DBWriter_.enqueue("messages",msg.SerializeAsString());
    }
  }
};

void ChatServer::sendMsgToUser(const std::string & Msg,const net::TcpConnectionPtr & conn) {
  Buffer sendBuff;

  int HeadLen = htonl(Msg.size());
  
  sendBuff.append(&HeadLen,sizeof(HeadLen));
  sendBuff.append(Msg);

  assert(conn);

  std::string buffmsg = sendBuff.retrieveAsString();
  
  conn->send(buffmsg);
}

bool ChatServer::isUserOnline(const std::string & user_email) {
  return onlineUserSet_.find(user_email) != onlineUserSet_.end();
}

void ChatServer::onOfflineMsg(const std::string & who,const std::string & msg) {
  DBWriter_.enqueue(offlineMessages + who,msg);
  LOG_INFO(who + ": new offline message.");
}

void ChatServer::offlineMsgConsumer(const TcpConnectionPtr & conn) {
  std::string offlineMessageKeyname = offlineMessages + conn->user_email();
  auto future = redis_.smembers(offlineMessageKeyname);
  redis_.sync_commit();
  auto reply = future.get();

  if(reply.is_array()) {
    auto members = reply.as_array();
    
    for(auto entry : members) {
      std::string message = entry.as_string();
      sendMsgToUser(message,conn);
      redis_.srem(offlineMessageKeyname,{message});
    }

    redis_.sync_commit();
  }
}

bool ChatServer::isUserExist(const std::string & user) {
  return m_allUserSet.find(user) != m_allUserSet.end();
}

void ChatServer::onGroupMessage(const std::string & group,Message & msgProto) {
  msgProto.set_isgroupmessage(true);

  if(!isGroupMember(msgProto.to(),msgProto.from())) {
    tellBlocked(msgProto.from(),msgProto.to());
    return ;
  }

  std::string msg = msgProto.SerializeAsString();
  auto future = redis_.smembers(groupMembers + group);
  redis_.sync_commit();
  auto reply = future.get();

  for(auto & entry : reply.as_array()) {
    std::string group_member = entry.as_string();
    if(group_member == msgProto.from()) {
      continue;
    }
    sendOrSave(group_member,msg);
    LOG_INFO("Group Message: In " + group + " to" + group_member);
  }
}

bool ChatServer::isGroupMessage(const std::string & who) {
  return groupSet_.find(who) != groupSet_.end();
}

void ChatServer::tellBlocked(const std::string & who,const std::string & by) {
  Message message;
  message.set_text(BLOCKED);
  message.set_from(by);
  message.set_isservice(true);

  sendOrSave(who,message.SerializeAsString());
}

bool ChatServer::isFriendsOf(const std::string & ofwho,const std::string & user) {
  auto future = redis_.sismember(serviceHandler_.friendSet + ofwho,{ user });
  redis_.sync_commit();
  auto reply = future.get();
  return reply.as_integer();
}

bool ChatServer::isGroupMember(const std::string & ofwho,const std::string & user) {
  return serviceHandler_.isUserGroupMember(user,ofwho);
}