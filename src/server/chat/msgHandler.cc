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
    return ;
  }

  bool isExists = false;
  redis_.sismember(allUserset,msg.to(),[&isExists](cpp_redis::reply & reply){ isExists = reply.as_integer(); });
  redis_.sync_commit();
  
  if(isExists == false) {
    LOG_ERROR("parseMessage: User Not Found!");
    return ;
  }


  if(msg.isservice()) {
    std::cout<<"action: "<<msg.text()<<"\n";
    std::cout<<"requestor: "<<msg.from()<<"\n";
    std::cout<<"obj: "<<msg.to()<<"\n";    

  } else {
    LOG_INFO(COLOR_YELLOW + msg.from() + COLOR_RESET + " says" + " to " + COLOR_YELLOW + msg.to() + COLOR_RESET + " : " + msg.text());

    if(isUserOnline(msg.to())) {
      assert(userHashConn[msg.to()]);
      sendMsgToUser(msg_str,userHashConn[msg.to()]);
    } else {
      /* 用户不在线 逻辑 */
      LOG_INFO("User is not online , message stored!");
      onOfflineMsg(msg.to(),msg_str);
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
  DBWriter_.enqueue("offlineMessages:"+who,msg);
  LOG_INFO(who + ": new offline message.");
}