#include"msg.h"
#include"msg.pb.h"
#include"Timestamp.h"
#include"EventLoop.h"
#include<ftxui/component/screen_interactive.hpp>
#include"ftp.h"

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

std::unordered_map<std::string,bool> NewMessageMap = {};

std::unordered_map<std::string,std::vector<messageinfo>> messageMap;

extern ftxui::ScreenInteractive MsgScreen;

extern std::unordered_map<std::string,int> MsgScreenScrollOffset;

extern int visible_lines;

extern std::vector<std::string> downloadable_files;

extern std::vector<ftxui::Component> download_buttons;

extern std::unordered_map<std::string,std::string> emailHashUserInfo;

extern ftxui::ScreenInteractive groupVerifyScreen;

MsgClient::MsgClient(FtpClient & ftp,const std::string & ip) : 
          chatServerfd_(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)),
          chatServerAddr_(ip,7070),
          ftpClient_(ftp),
          heart(chatServerfd_),
          pool_(1)
{
  initServiceCallbackMap();
  heart.setSendMessageCallback([this]{
    safeSend(HEARTBEAT_MSG);
  });
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
  net::EventLoop loop;
  loop_ = &loop;
  net::Channel recvChannel(&loop,chatServerfd_);
  recvChannel.setReadCallback([this](Timestamp){ onMessage(); });
  recvChannel.enableReading();
  
  loop.loop();
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
  if(msg == HEARTBEAT_BACK_MSG) {
    heart.recvAck();
  }
  
  Message msgProto;
  msgProto.ParseFromString(msg);

  if(msgProto.isservice()) {
    doService(msgProto);
  } else {
    if(msgProto.isgroupmessage()) {
      NewMessageMap[msgProto.to()] = true;
      messageMap[msgProto.to()].push_back({emailHashUserInfo[msgProto.from()],msgProto.text(),msgProto.timestamp()});
      MsgScreenScrollOffset[msgProto.to()] = std::max(0, static_cast<int>(messageMap[msgProto.to()].size()) - visible_lines);
    } else {
      NewMessageMap[msgProto.from()] = true;
      messageMap[msgProto.from()].push_back({emailHashUserInfo[msgProto.from()],msgProto.text(),msgProto.timestamp()});
      MsgScreenScrollOffset[msgProto.from()] = std::max(0, static_cast<int>(messageMap[msgProto.from()].size()) - visible_lines);
    }
    MsgScreen.PostEvent(ftxui::Event::Custom);
  }
}

void MsgClient::doService(Message msgProto) {
  if(msgProto.from() == ADDFRIEND_BACK) {
    doAddFriendBack(msgProto);
  } else if(msgProto.text() == PULL_FRIEND_LIST) {
    pullFriendList(true,msgProto);
  } else if(msgProto.text() == VERI_FRIEND_BACK) {
    pullFriendList();
  } else if(msgProto.text() == FRIEND_BE_ONLINE) {
    doUpdateFriendState(msgProto,true);
  } else if(msgProto.text() == FRIEND_BE_OFFLINE) {
    doUpdateFriendState(msgProto,false);
  } else if(msgProto.text() == PULL_GROUP_LIST) {
    pullGroupList(true,msgProto);
  } else if(msgProto.from() == ADDGROUP_BACK) {
    doAddGroupBack(msgProto);
  } else if(msgProto.text() == PULL_GROUP_MEMBERS) {
    pullGroupMembers(true,{},msgProto);
  } else if(msgProto.text() == GROUP_EXIST) {
    doGroupExist(msgProto);
  } else if(msgProto.text() == PULL_DL_LIST) {
    doPullDlList(msgProto);
  } else if(msgProto.text() == PULL_ALL_USERS) {
    pullAllUsers(true,msgProto);
  } else if(msgProto.text() == PULL_GROUP_OWNER) {
    pullGroupOwner(true,{},msgProto);
  } else if(msgProto.text() == PULL_GROUP_OPS) {
    pullGroupOPs(true,{},msgProto);
  } else {
    if(serviceCallbackMap.find(msgProto.text()) != serviceCallbackMap.end())
    serviceCallbackMap[msgProto.text()](msgProto);
  }
}

void MsgClient::initServiceCallbackMap() {
  serviceCallbackMap[ADD_FRIEND] = std::bind(&MsgClient::doAddFriend,this,std::placeholders::_1);
  serviceCallbackMap[DEL_FRIEND_SUCCESS] = std::bind(&MsgClient::doDeleteFriend,this,std::placeholders::_1);
  serviceCallbackMap[CREATE_GROUP] = std::bind(&MsgClient::doCreateGroup,this,std::placeholders::_1);
  serviceCallbackMap[ADD_GROUP] = std::bind(&MsgClient::doAddGroup,this,std::placeholders::_1);
  serviceCallbackMap[VERI_GROUP_SUCCESS] = std::bind(&MsgClient::doVeriGroup,this,std::placeholders::_1);
  serviceCallbackMap[MEMBER_QUIT_GROUP] = std::bind(&MsgClient::doGroupMemberQuit,this,std::placeholders::_1);
  serviceCallbackMap[QUIT_GROUP_BACK] = std::bind(&MsgClient::doQuitGroup,this,std::placeholders::_1);
  serviceCallbackMap[BLOCKED] = std::bind(&MsgClient::doBlockedMessage,this,std::placeholders::_1);
  serviceCallbackMap[UPLOAD_FILE] = std::bind(&MsgClient::doRecvFile,this,std::placeholders::_1);
  serviceCallbackMap[GROUP_EXIST] = std::bind(&MsgClient::doGroupExist,this,std::placeholders::_1);
}

void MsgClient::doAddFriendBack(const Message & msgProto) {
  if(msgProto.text() == ADD_FRIEND_SEND_SUCCESS) {
    show_info = "好友申请已发送!";
  } else if(msgProto.text() == ADD_FRIEND_SEND_FAILED) {
    show_info = "用户不存在!";
  }
  std::thread([&]{ sleep(2); show_info = ""; }).detach();
}

std::mutex friendRequestsLock;

void MsgClient::doAddFriend(const Message & msgProto) {
  show_info2 = "新的好友申请!";
  std::lock_guard<std::mutex> lock(friendRequestsLock);
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
  std::thread([&]{ sleep(2); show_info4 = ""; }).detach();
}

void MsgClient::doCreateGroup(const Message & msgProto) {
  show_info3 = "已加入新群聊" + msgProto.from();
  pullGroupList();
  std::thread([&]{ sleep(2); show_info4 = ""; }).detach();
}

void MsgClient::doAddGroupBack(const Message & msgProto) {
  if(msgProto.text() == ADD_GROUP_SEND_SUCCESS) {
    show_info4 = "加群申请已发送";
  } else {
    show_info4 = "群聊不存在!";
  } 
  std::thread([&]{ sleep(2); show_info4 = ""; }).detach();
}

std::mutex groupRequestsLock;

void MsgClient::doAddGroup(const Message & msgProto) {
  std::lock_guard<std::mutex> lock(groupRequestsLock);
  applications.push_back({msgProto.from(),msgProto.to()});
  show_info3 = "有新的加群申请!";
  groupVerifyScreen.PostEvent(ftxui::Event::Custom);
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

void MsgClient::doGroupMemberQuit(const Message & msg) {
  show_info4 = "用户 " + msg.from() + " 退出了您的群聊 " + msg.to();
  std::thread([&]{ sleep(2); show_info4 = ""; }).detach();
  pullGroupList();
}

void MsgClient::doQuitGroup(const Message & msg) {
  if(msg.to() == QUIT_GROUP_SUCCESS) {
    show_info4 = "已退出群聊" + msg.from();
  } else if(msg.to() == QUIT_GROUP_FAILED) {
    show_info4 = "退出失败! 您不能退出自己的群聊。 尝试解散群聊!";
  }
  std::thread([&]{ sleep(2); show_info4 = ""; }).detach();
  pullGroupList();
}

bool MsgClient::isPeerGroup() const {
  return isPeerGroup_;
}

void MsgClient::CancelAccount(const std::string & account) {
  SerializeSend(CANCEL,LocalEmail_,LocalEmail_);
  std::filesystem::remove("./history/" + LocalEmail_ + ".data");
}

void MsgClient::enMapYouMessage(Message msgProto) {
  if(msgProto.isgroupmessage()) {
    messageMap[msgProto.to()].push_back({"You",msgProto.text(),msgProto.timestamp()});
    MsgScreenScrollOffset[msgProto.to()] = std::max(0, static_cast<int>(messageMap[msgProto.to()].size()) - visible_lines);
  } else {
    messageMap[msgProto.from()].push_back({"You",msgProto.text(),msgProto.timestamp()});
    MsgScreenScrollOffset[msgProto.from()] = std::max(0, static_cast<int>(messageMap[msgProto.from()].size()) - visible_lines);
  }
  MsgScreen.PostEvent(ftxui::Event::Custom);
}

void MsgClient::doBlockedMessage(Message message) {
  if(message.from().find('@') != message.from().npos) {
    messageMap[message.from()].push_back({"系统","消息已发出，但被对方拒收了",Timestamp::now().microSecondsSinceEpoch()});
  } else {
    messageMap[message.from()].push_back({"系统","您已不是该群成员或该群不存在!",Timestamp::now().microSecondsSinceEpoch()});
    Message msg;
    msg.set_from(message.from());
    msg.set_to(QUIT_GROUP_SUCCESS);
    doQuitGroup(msg);
  }
  
  MsgScreenScrollOffset[message.from()] = std::max(0, static_cast<int>(messageMap[message.from()].size()) - visible_lines);

  MsgScreen.PostEvent(ftxui::Event::Custom);
}

void MsgClient::doRecvFile(Message msgProto) {
  pullDownloadList(msgProto.from(),msgProto.to());
  
  if(msgProto.isgroupmessage()) {
    std::string fileName = msgProto.to();
    messageMap[msgProto.from()].push_back({"系统","你收到了来自" + msgProto.from() + "的文件。 请前往文件页面接收。",ilib::base::Timestamp::now().microSecondsSinceEpoch()});
    messageMap[msgProto.from()].push_back({"系统","文件名: " + fileName,ilib::base::Timestamp::now().microSecondsSinceEpoch()});
    show_info2 = "你收到了来自" + msgProto.from() + "的文件。";
  } else {
    std::string fileName = msgProto.to();
    messageMap[msgProto.from()].push_back({"系统","你收到了来自" + msgProto.from() + "的文件。 请前往文件页面接收。",ilib::base::Timestamp::now().microSecondsSinceEpoch()});
    messageMap[msgProto.from()].push_back({"系统","文件名: " + fileName,ilib::base::Timestamp::now().microSecondsSinceEpoch()});
    show_info = "你收到了来自" + msgProto.from() + "的文件。";
  }
  
  if(msgProto.isgroupmessage()) {
    MsgScreenScrollOffset[msgProto.to()] = std::max(0, static_cast<int>(messageMap[msgProto.to()].size()) - visible_lines);
  } else {
    MsgScreenScrollOffset[msgProto.from()] = std::max(0, static_cast<int>(messageMap[msgProto.from()].size()) - visible_lines);
  }

  MsgScreen.PostEvent(ftxui::Event::Custom);
}

void MsgClient::doGroupExist(Message message) {
  show_info4 = "创建失败! 已经存在一个同名的群聊了!";
  std::thread([&]{ sleep(2); show_info4 = ""; }).detach();
}

void MsgClient::doPullDlList(Message msgProto) {
  download_buttons.clear();
  downloadable_files.clear();
  for(int i = 0;i<msgProto.args_size();i++) {
    downloadable_files.push_back(msgProto.args(i));
  }
  MsgScreen.PostEvent(ftxui::Event::Custom);
}

bool MsgClient::isGroupOwner(std::string user,std::string group) {
  if(groupHashOwner[group] == user) {
    return true;
  } else {
    return false;
  }
}

bool MsgClient::isGroupOp(std::string user,std::string group) {
  for(auto e : groupOPs[group]) {
    if(user == e) {
      return true;
    }
  }

  return false;
}