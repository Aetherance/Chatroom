#include"ChatServer.h"
#include"Buffer.h"
#include<assert.h>
#include"logger.h"
#include"msg.pb.h"

extern std::unordered_map<std::string,net::TcpConnectionPtr> userHashConn;

using namespace net;

void ChatServer::parseMessage(const std::string & msg_str,const net::TcpConnectionPtr & conn) {
  Message msg;
  if(!msg.ParseFromString(msg_str)) {
    perror("Protobuf: ChatServer");
    return ;
  }

  if(msg.from() == msg.to() && msg.from() == "SET_CONN_USER") {
    conn->set_user_email(msg.text());
    userHashConn[msg.text()] = conn;
    redis_.sadd(onlineUserSet,{msg.text()});
    offlineMsgConsumer(conn);
    serviceHandler_.FriendBeOnline(conn);
    return ;
  }

  if(isGroupMessage(msg.to())) {
    onGroupMessage(msg.to(),msg_str);
  } else {
    bool isExists = isUserExist(msg.to());
  
    if(isExists == false && !msg.isservice()) {
      LOG_ERROR("parseMessage: User Not Found!");
    }

    if(msg.isservice()) {
      /* 当isservice为真 Message的text字段代表服务的类型 */
      serviceCallBacks_[msg.text()](conn,msg);

    } else if( !msg.isservice() && isExists) {
      LOG_INFO(COLOR_YELLOW + msg.from() + COLOR_RESET + " says" + " to " + COLOR_YELLOW + msg.to() + COLOR_RESET + " : " + msg.text());

      if(isUserOnline(msg.to())) {
        assert(userHashConn[msg.to()]);
        /* to字段存储接收端信息 */
        sendMsgToUser(msg_str,userHashConn[msg.to()]);
      } else {
        /* 用户不在线 逻辑 */
        LOG_INFO("User is not online , message stored!");
        onOfflineMsg(msg.to(),msg_str);
      }
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

  DBWriter_.enqueue("messages",Msg);
}

bool ChatServer::isUserOnline(const std::string & user_email) {
  auto exists_future = redis_.sismember(onlineUserSet, user_email);
  redis_.sync_commit();
  cpp_redis::reply reply = exists_future.get();
  if(reply.is_integer()) {
    if(reply.as_integer()) {
      return true;
    } else {
      return false;
    }
  }
  return false;
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
  bool isExists = false;
  redis_.sismember(allUserset,user,[&isExists](cpp_redis::reply & reply){ isExists = reply.as_integer(); });
  redis_.sync_commit();
  return isExists;
}

void ChatServer::onGroupMessage(const std::string & group,const std::string & msg) {
  auto future = redis_.smembers(groupMembers + group);
  redis_.sync_commit();
  auto reply = future.get();

  for(auto & entry : reply.as_array()) {
    std::string group_member = entry.as_string();
    if(isUserOnline(group_member)) {
      sendMsgToUser(msg,userHashConn[group_member]);
    } else {
      onOfflineMsg(group_member,msg);
    }
  }
}

bool ChatServer::isGroupMessage(const std::string & who) {
  auto future = redis_.sismember(allGroupSet,who);
  redis_.sync_commit();
  auto reply = future.get();
  if(reply.as_integer()) {
    return true;
  } else {
    return false;
  }
}