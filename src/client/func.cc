#include"msg.h"
#include<jsoncpp/json/json.h>
#include"responsecode.h"
#include"Timestamp.h"
#include"msg.pb.h"
#include<ftxui/component/screen_interactive.hpp>

extern std::vector<Friend> friends;

extern std::vector<Group> groups;

extern ftxui::ScreenInteractive FriendListScreen;

extern ftxui::ScreenInteractive GroupListScreen;

extern ftxui::ScreenInteractive MsgScreen;

extern ftxui::ScreenInteractive findScreen;

extern std::string show_info2;

std::vector<Friend> users;

std::unordered_map<std::string,std::string> emailHashUserInfo;

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

void MsgClient::blockFriend(const std::string & requestor,const std::string & obj) {
  SerializeSend(BLOCK,requestor,obj);
}

void MsgClient::addGroup(const std::string & requestor,const std::string & obj) {
  SerializeSend(ADD_GROUP,requestor,obj);
}

void MsgClient::quitGroup(const std::string & requestor,const std::string & obj) {
  SerializeSend(QUIT_GROUP,requestor,obj);
}

void MsgClient::breakGroup(const std::string & requestor,const std::string & obj) {
  SerializeSend(BREAK_GROUP,requestor,obj);
}

void MsgClient::unBlock(const std::string & requestor,const std::string & obj) {
  SerializeSend(UNBLOCK,requestor,obj);
}

void MsgClient::pullFriendList(bool isRecv,Message msg) {
  if(!isRecv) {
    SerializeSend(PULL_FRIEND_LIST,LocalEmail(),PULL_FRIEND_LIST);
    return;
  } 
  
  std::vector<std::string> user_email;
  std::vector<std::string> user_name;
  
  friends.clear();
  
  for(int i = 0;i<msg.args_size();i++) {
    std::string arg = msg.args(i);
    user_email.push_back(arg);
    int pos = user_email[i].find('\n');
    bool isOnline = user_email[i][pos+1] == 'O';
    user_name.emplace_back(user_email[i].begin() + pos + 2,user_email[i].begin() + user_email[i].size());
    user_email[i].resize(pos);
    friends.push_back({user_email[i],user_name[i],isOnline});
    emailHashUserInfo[user_email[i]] = user_name[i];
  }
  

  if(friends.empty()) {
    show_info2 = "前往\"发现\"或搜索邮箱以添加您的第一个好友! ";
  }

  FriendListScreen.PostEvent(ftxui::Event::Custom);
}

void MsgClient::createGroup(const std::string & creator,const std::string & group,const std::vector<std::string> & members) {
  Message msg;
  msg.set_from(creator);
  msg.set_to(group);
  msg.set_text(CREATE_GROUP);
  msg.set_isservice(true);
  for(auto & str : members) {
    msg.add_args(str);
  }
  std::string message_str = msg.SerializeAsString();
  
  safeSend(message_str);
}

void MsgClient::pullGroupList(bool isRecv,Message msg) {
  if(!isRecv) {
    SerializeSend(PULL_GROUP_LIST,LocalEmail(),PULL_GROUP_LIST);
    return;
  } 
  
  groups.clear();
  for(int i = 0;i<msg.args_size();i++) {
    std::string arg = msg.args(i);
    groups.push_back({arg});
  }
  
  pullGroupMembers();
  
  GroupListScreen.PostEvent(ftxui::Event::Custom);
}

std::string extractSubstring(const std::string& str) {
    int len = str.length();
    if (len < 2) {
        return "";
    }
    
    int pos = -1;  // 用于记录找到的']'的位置
    for (int i = len - 2; i >= 0; i--) {
        if (str[i] == '[') {
            if (i < len - 2) {
                pos = i;
                break;
            }
        }
    }
    if (pos == -1) {
        return "";
    }
    
    int start = pos + 1;
    int length = (len - 2) - (pos + 1);

    return str.substr(start, length);
}

void MsgClient::pullGroupMembers(bool isRecv,std::string request_group,Message msg) {
  if(!isRecv) {
    SerializeSend(PULL_GROUP_MEMBERS,request_group,PULL_GROUP_MEMBERS);
  } else {
    std::string group = msg.to();
    groupMembers[group].clear();
    for(int i = 0;i<msg.args_size();i++) {
      groupMembers[group].push_back(msg.args(i));
      groupMembersEmail[group].push_back(extractSubstring(msg.args(i)));
    }
  }

  MsgScreen.PostEvent(ftxui::Event::Custom);
}

void MsgClient::setOP(const std::string & user,const std::string & group) {
  Json::Value root;
  root["useremail"] = user;
  root["group"] = group;
  Json::StreamWriterBuilder writer;
  std::string info = Json::writeString(writer,root);
  
  Message setOp;
  setOp.set_from(LocalEmail_);
  setOp.set_text(SET_OP);
  setOp.set_to(info);
  setOp.set_isservice(true);

  std::string message = setOp.SerializeAsString();

  safeSend(message);

  pullGroupMembers();

  MsgScreen.PostEvent(ftxui::Event::Custom);
}

void MsgClient::deOP(const std::string & user,const std::string & group) {
  Json::Value root;
  root["useremail"] = user;
  root["group"] = group;
  Json::StreamWriterBuilder writer;
  std::string info = Json::writeString(writer,root);
  
  Message setOp;
  setOp.set_from(LocalEmail_);
  setOp.set_text(DE_OP);
  setOp.set_to(info);
  setOp.set_isservice(true);

  std::string message = setOp.SerializeAsString();

  safeSend(message);

  pullGroupMembers();

  MsgScreen.PostEvent(ftxui::Event::Custom);
}

void MsgClient::rmGroupMember(const std::string & who,const std::string & group) {
  Json::Value root;
  root["useremail"] = who;
  root["group"] = group;
  Json::StreamWriterBuilder writer;
  std::string info = Json::writeString(writer,root);

  Message pullGM;
  pullGM.set_from(LocalEmail_);
  pullGM.set_text(RM_GROUP_MEM);
  pullGM.set_to(info);
  pullGM.set_isservice(true);

  std::string message = pullGM.SerializeAsString();

  safeSend(message);

  pullGroupMembers();
}

void MsgClient::pullDownloadList(const std::string & receiver,const std::string sender) {
  SerializeSend(PULL_DL_LIST,receiver,sender);
}

void MsgClient::pullAllUsers(bool isRecv ,Message msg) {
  if(!isRecv) {
    SerializeSend(PULL_ALL_USERS,LocalEmail(),PULL_ALL_USERS);
    return;
  } 
  
  std::vector<std::string> user_email;
  std::vector<std::string> user_name;
  
  users.clear();

  for(int i = 0;i<msg.args_size();i++) {
    std::string arg = msg.args(i);
    user_email.push_back(arg);
    int pos = user_email[i].find('\n');
    user_name.emplace_back(user_email[i].begin() + pos + 1,user_email[i].begin() + user_email[i].size());
    user_email[i].resize(pos);

    users.push_back({user_email[i],user_name[i],0});
  }
  findScreen.PostEvent(ftxui::Event::Custom);
}

void MsgClient::pullGroupOwner(bool isRecv , std::string request_group ,Message msg) {
  if(!isRecv) {
    SerializeSend(PULL_GROUP_OWNER,request_group,PULL_GROUP_OWNER);
  } else {
    groupHashOwner[msg.from()] = msg.to();
  }
}

void MsgClient::pullGroupOPs(bool isRecv , std::string request_group ,Message msg) {
  if(!isRecv) {
    SerializeSend(PULL_GROUP_OPS,request_group,PULL_GROUP_OPS);
  } else {
    for(int i = 0;i<msg.args_size();i++) {
      groupOPs[msg.from()].push_back(msg.args(i));
    }
  }
}