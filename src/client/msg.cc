#include"msg.h"
#include"msg.pb.h"
#include"Timestamp.h"
#include"EventLoop.h"
#include<ftxui/component/screen_interactive.hpp>

using namespace ilib;

#define TCP_HEAD_LEN sizeof(int)

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
  connSetMsg.set_text(email_);
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
  msg.set_from(email_);
  msg.set_to(who);
  msg.set_text(msgtext);
  msg.set_timestamp(now.microSecondsSinceEpoch());
  msg.set_isservice(false);

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

  // std::lock_guard<std::mutex> lock(messageMapMutex); // 加锁

  messageMap[msgProto.from()].push_back({msgProto.from(),msgProto.text(),msgProto.timestamp()});
  MsgScreen.PostEvent(ftxui::Event::Custom);
  MsgScreenScrollOffset = std::max(0, static_cast<int>(messageMap[msgProto.from()].size()) - visible_lines);
}