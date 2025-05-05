#include"msg.h"
#include"msg.pb.h"
#include"Timestamp.h"
#include"EventLoop.h"

using namespace ilib;

#define TCP_HEAD_LEN sizeof(int)

MsgClient::MsgClient() : chatServerfd_(socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)),
                         chatServerAddr_("localhost",7070)   
{

}

MsgClient::~MsgClient() {

}

void MsgClient::connect() {
  ::connect(chatServerfd_,(sockaddr*)&chatServerAddr_.getSockAddr(),chatServerAddr_.getSockLen());
}

void MsgClient::updatePeer(const std::string &newPeer) {
  msgPeer_ = newPeer;
}

void MsgClient::sendMsgTo(const std::string & who,const std::string & msgtext) {
  Message msg;
  base::Timestamp now = base::Timestamp::now();
  msg.set_from(username_);
  msg.set_to(who);
  msg.set_text(msgtext);
  msg.set_timestamp(now.microSecondsSinceEpoch());
  std::string message = msg.SerializeAsString();
  
  int LengthHead = htonl(message.size());
  ::send(chatServerfd_,&LengthHead,sizeof(LengthHead),0);
  ::send(chatServerfd_,message.data(),message.size(),0);
  std::cout<<msg.text()<<"\n";
}

void MsgClient::recvMsgLoop() {
  net::EventLoop recvLoop;
  net::Channel recvChannel(&recvLoop,chatServerfd_);
  recvChannel.setReadCallback([this](Timestamp){ onMessage(); });
  recvChannel.enableReading();
  
  recvLoop.loop();
}

void MsgClient::onMessage() {
  std::cout<<"onMessage!\n";
  recvBuff_.readFd(chatServerfd_,nullptr);
  while(recvBuff_.readableBytes() >= TCP_HEAD_LEN) {
    const char * data = recvBuff_.peek();
    int headLen = ntohl( *(int *)data );
    if(recvBuff_.readableBytes() >= headLen + TCP_HEAD_LEN) {
      recvBuff_.retrieve(TCP_HEAD_LEN);
      std::string msg(recvBuff_.peek(),headLen);
      std::cout<<msg<<"\n"; // 暂时使用cout
      recvBuff_.retrieve(headLen);
    } else {
      break;
    }
  }
}