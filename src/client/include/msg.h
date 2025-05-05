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
  void sendMsgTo(const std::string & who,const std::string & msg);
  void updatePeer(const std::string &newPeer);
  void recvMsgLoop();
  void onMessage();
private:
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