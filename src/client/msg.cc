#include"msg.h"

MsgClient::MsgClient() {

}

MsgClient::~MsgClient() {

}

void MsgClient::updatePeer(const std::string &newPeer) {
  msgPeer_ = newPeer;
}

void MsgClient::sendMsgTo(const std::string & who,const std::string & msgtext) {
  
}