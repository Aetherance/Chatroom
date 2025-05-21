#include"msg.h"
#include"msg.pb.h"
#include"Timestamp.h"
#include"EventLoop.h"
#include<ftxui/component/screen_interactive.hpp>

using namespace ilib;

#define TCP_HEAD_LEN sizeof(int)

extern std::string show_info;

extern std::string show_info2;

extern std::string show_info3;

extern std::string show_info4;

extern std::vector<std::string> friendRequests;

extern std::vector<Friend>friends;

extern std::vector<Group> groups;

extern std::vector<GroupApplication> applications;

std::unordered_map<std::string,std::vector<messageinfo>> messageMap;

extern ftxui::ScreenInteractive MsgScreen;
extern int MsgScreenScrollOffset;
extern int visible_lines;

MsgClient::MsgClient() : 
          chatServerfd_(socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)),
          chatServerAddr_("localhost",7070)   
{

}

MsgClient::~MsgClient() {

}

void MsgClient::connect() {
  ::connect(chatServerfd_,(sockaddr*)&chatServerAddr_.getSockAddr(),chatServerAddr_.getSockLen());
  Message connSetMsg;
  connSetMsg.set_from("SET_CONN_USER");
  connSetMsg.set_to("SET_CONN_USER");
  connSetMsg.set_text(LocalEmail_);
  safeSend(connSetMsg.SerializeAsString());
}

void MsgClient::updatePeer(const std::string &newPeerEmail,const std::string & newPeerUsername) {
  msgPeerEmail_ = newPeerEmail;
  msgPeerUsername_ = newPeerUsername;
}

void MsgClient::safeSend(const std::string & message) {
  uint32_t msg_len = static_cast<uint32_t>(message.size());
  uint32_t LengthHead = htonl(msg_len);
  ::send(chatServerfd_, &LengthHead, sizeof(LengthHead), 0);
  ::send(chatServerfd_,message.data(),message.size(),0);
}

void MsgClient::sendMsgTo(const std::string & who,const std::string & msgtext) {
  Message msg;
  base::Timestamp now = base::Timestamp::now();
  msg.set_from(LocalEmail_);
  msg.set_to(who);
  msg.set_text(msgtext);
  msg.set_timestamp(now.microSecondsSinceEpoch());
  msg.set_isservice(false);
  msg.set_isgroupmessage(false);

  std::string message = msg.SerializeAsString();
  
  constexpr uint32_t MAX_MSG_SIZE = 10 * 1024 * 1024;  // 10MB
    if (message.size() > MAX_MSG_SIZE) {
      return;
  }

  safeSend(message);
}

void MsgClient::recvMsgLoop() {
  net::EventLoop recvLoop;
  net::Channel recvChannel(&recvLoop,chatServerfd_);
  recvChannel.setReadCallback([this](Timestamp){ onMessage(); });
  recvChannel.enableReading();
  
  recvLoop.loop();
}

void MsgClient::onMessage() {
  recvBuff_.readFd(chatServerfd_,nullptr);
  while(recvBuff_.readableBytes() >= TCP_HEAD_LEN) {
    const char * data = recvBuff_.peek();
    int headLen = ntohl( *(int *)data );
    if(recvBuff_.readableBytes() >= headLen + TCP_HEAD_LEN) {
      recvBuff_.retrieve(TCP_HEAD_LEN);
      std::string msg(recvBuff_.peek(),headLen);
      parseMsg(msg);
      recvBuff_.retrieve(headLen);
    } else {
      break;
    }
  }
}

void MsgClient::parseMsg(std::string msg) {
  Message msgProto;
  msgProto.ParseFromString(msg);

  if(msgProto.isservice()) {
    doService(msgProto);
  } else {
    if(msgProto.isgroupmessage()) {
      messageMap[msgProto.to()].push_back({msgProto.from(),msgProto.text(),msgProto.timestamp()});
      MsgScreenScrollOffset = std::max(0, static_cast<int>(messageMap[msgProto.to()].size()) - visible_lines);
    } else {
      messageMap[msgProto.from()].push_back({msgProto.from(),msgProto.text(),msgProto.timestamp()});
      MsgScreenScrollOffset = std::max(0, static_cast<int>(messageMap[msgProto.from()].size()) - visible_lines);
    }
    MsgScreen.PostEvent(ftxui::Event::Custom);
  }
}

void MsgClient::doService(Message msgProto) {
  if(msgProto.from() == ADDFRIEND_BACK) {
    doAddFriendBack(msgProto);    
  } else if(msgProto.text() == ADD_FRIEND) {
    doAddFriend(msgProto);
  } else if(msgProto.from() == PULL_FRIEND_LIST) {
    pullFriendList(true,msgProto);
  } else if(msgProto.text() == VERI_FRIEND_BACK) {
    pullFriendList();
  } else if(msgProto.text() == FRIEND_BE_ONLINE) {
    doUpdateFriendState(msgProto,true);
  } else if(msgProto.text() == FRIEND_BE_OFFLINE) {
    doUpdateFriendState(msgProto,false);
  } else if(msgProto.text() == DEL_FRIEND_SUCCESS) {
    doDeleteFriend(msgProto);
  } else if(msgProto.text() == CREATE_GROUP) {
    doCreateGroup(msgProto);
  } else if(msgProto.from() == PULL_GROUP_LIST) {
    pullGroupList(true,msgProto);
  } else if(msgProto.from() == ADDGROUP_BACK) {
    doAddGroupBack(msgProto);
  } else if(msgProto.text() == ADD_GROUP) {
    doAddGroup(msgProto);
  } else if(msgProto.text() == VERI_GROUP_SUCCESS) {
    doVeriGroup(msgProto);
  }
}

void MsgClient::doAddFriendBack(const Message & msgProto) {
  if(msgProto.text() == ADD_FRIEND_SEND_SUCCESS) {
    show_info = "好友申请已发送!";
  } else if(msgProto.text() == ADD_FRIEND_SEND_FAILED) {
    show_info = "用户不存在!";
  }
  std::thread([&]{ sleep(2); show_info = ""; }).detach();
}

void MsgClient::doAddFriend(const Message & msgProto) {
  show_info2 = "新的好友申请!";
  friendRequests.push_back(msgProto.from());
}

void MsgClient::doUpdateFriendState(const Message & msgProto,bool isOnline) {
  for(auto & Friend : friends) {
    if(Friend.email == msgProto.from()) {
      Friend.isOnline = isOnline;
    }
  }
}

void MsgClient::doDeleteFriend(const Message & msgProto) {
  friends.clear();
  pullFriendList();
  show_info = "您已和" + msgProto.from() + "的好友关系已断开!";
}

void MsgClient::doCreateGroup(const Message & msgProto) {
  show_info3 = "已加入新群聊" + msgProto.from();
  pullGroupList();
}

void MsgClient::doAddGroupBack(const Message & msgProto) {
  if(msgProto.text() == ADD_GROUP_SEND_SUCCESS) {
    show_info4 = "加群申请已发送";
  } else {
    show_info4 = "群聊不存在!";
  } 
  std::thread([&]{ sleep(2); show_info4 = ""; }).detach();
}

void MsgClient::doAddGroup(const Message & msgProto) {
  applications.push_back({msgProto.from(),msgProto.to()});
  show_info3 = "有新的加群申请!";
}

void MsgClient::verifyGroup(const std::string & who, const std::string & group) {
  SerializeSend(VERI_GROUP,who,group);
}

void MsgClient::doVeriGroup(const Message & msg) {
  if(msg.to() == ENTERING_NEW_GROUP) {
    groups.push_back({msg.from()});
    show_info4 = "您已经加入" + msg.from();
    std::thread([&]{ sleep(2); show_info4 = ""; }).detach();
    pullGroupList();
  } else if(msg.to() == VERI_GROUP_SUCCESS) {
    show_info4 = msg.from() + "加入了您的群聊";
    std::thread([&]{ sleep(2); show_info4 = ""; }).detach();
  }
}