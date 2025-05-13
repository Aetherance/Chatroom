#include"msg.h"
#include<jsoncpp/json/json.h>
#include"responsecode.h"
#include"Timestamp.h"
#include"msg.pb.h"

void MsgClient::SerializeSend(const std::string action,const std::string & Requestor,const std::string & obj,const std::vector<std::string>& args) {
  Message ServiceMsg;
  ServiceMsg.set_text(action);
  ServiceMsg.set_from(Requestor);
  ServiceMsg.set_to(obj);
  ServiceMsg.set_timestamp(ilib::base::Timestamp::now().microSecondsSinceEpoch());
  ServiceMsg.set_isservice(true);

  std::string ServiceMsgStr = ServiceMsg.SerializeAsString();

  safeSend(ServiceMsgStr);
}

void MsgClient::addFriend(const std::string & requestor,const std::string & obj) {
  SerializeSend(ADD_FRIEND,requestor,obj);
}

void MsgClient::deleteFriend(const std::string & requestor,const std::string & obj) {
  SerializeSend(DEL_FRIEND,requestor,obj);
}

void MsgClient::verifyFriend(const std::string & requestor,const std::string & obj) {
  SerializeSend(VER_FRIEND,requestor,obj);
}

void MsgClient::blackoutFriend(const std::string & requestor,const std::string & obj) {
  SerializeSend(BLACK_OUT,requestor,obj);
}

void MsgClient::blockFriend(const std::string & requestor,const std::string & obj) {
  SerializeSend(BLOCK,requestor,obj);
}

void MsgClient::addGroup(const std::string & requestor,const std::string & obj) {
  SerializeSend(ADD_GROUP,requestor,obj);
}

void MsgClient::quitGroup(const std::string & requestor,const std::string & obj) {
  SerializeSend(QUIT_GROUP,requestor,obj);
}