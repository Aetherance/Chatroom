#include"msg.h"
#include<jsoncpp/json/json.h>
#include"responsecode.h"
#include"Timestamp.h"
#include"msg.pb.h"
#include<ftxui/component/screen_interactive.hpp>

extern std::vector<Friend> friends;

extern ftxui::ScreenInteractive FriendListScreen;

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

void MsgClient::pullFriendList(bool isRecv,Message msg) {
  if(!isRecv) {
    SerializeSend(PULL_FRIEND_LIST,LocalEmail(),PULL_FRIEND_LIST);
    return;
  } 

  std::vector<std::string> user_email;
  std::vector<std::string> user_name;
  
  for(int i = 0;i<msg.args_size();i++) {
    std::string arg = msg.args(i);
    user_email.push_back(arg);
    int pos = user_email[i].find('\n');
    bool isOnline = user_email[i][pos+1] == 'O';
    user_name.emplace_back(user_email[i].begin() + pos + 2,user_email[i].begin() + user_email[i].size());
    user_email[i].resize(pos);
    friends.push_back({user_email[i],user_name[i],isOnline});
  }

  FriendListScreen.PostEvent(ftxui::Event::Custom);
}