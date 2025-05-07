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
  
  bool isExists = false;
  redis_.sismember(allUserset,msg.to(),[&isExists](cpp_redis::reply & reply){ isExists = reply.as_integer(); });
  redis_.sync_commit();
  
  if(isExists == false) {
    LOG_ERROR("parseMessage: User Not Found!");
    return ;
  }

  bool isOnline = false;
  redis_.sismember(onlineUserSet,msg.to(),[&isOnline](cpp_redis::reply & reply){ isOnline = reply.as_integer(); });
  redis_.sync_commit();

  if(isOnline == false) {
    LOG_ERROR("parseMessage: User is offline now!");
    return ;
  }

  LOG_INFO(COLOR_YELLOW + msg.from() + COLOR_RESET + " says" + " to " + COLOR_YELLOW + msg.to() + COLOR_RESET + " : " + msg.text());

  assert(userHashConn[msg.to()]);

  if(isUserOnline(msg.to())) {
    sendMsgToUser(msg_str,userHashConn[msg.to()]);
  } else {
    /* 用户不在线 逻辑 */
    onOfflineMsg(msg.to(),msg_str);
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
  
}