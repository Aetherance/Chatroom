#include<string>
#include"arpa/inet.h"
#include"InetAddress.h"
#include"Buffer.h"

class MsgClient
{
public:
  MsgClient();
  ~MsgClient();
  void connect();
  inline void sendMsgPeer(const std::string & msg) { sendMsgTo(msgPeer_,msg); }
  void updatePeer(const std::string &newPeer);
  void recvMsgLoop();
  void onMessage();

  void setEmail(const std::string & email) { email_ = email; }
private:
  void sendMsgTo(const std::string & who,const std::string & msg);
  void echoMsg(const std::string & sourceMsg,std::string & echoMsg);

  /* 用户信息 */
  std::string username_;
  std::string email_;

  /* 当前聊天对象信息 */
  /* 群号或邮箱 */
  std::string msgPeer_;
  
  ilib::net::InetAddress chatServerAddr_;
  int chatServerfd_;

  ilib::net::Buffer recvBuff_;
};