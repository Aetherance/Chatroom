#include<string>

class MsgClient
{
public:
  MsgClient();
  ~MsgClient();
  void sendMsgTo(const std::string & who,const std::string & msg);
  void updatePeer(const std::string &newPeer);
  private:
  /* 用户信息 */
  std::string username_;
  std::string email_;

  /* 当前聊天对象信息 */
  /* 群号或邮箱 */
  std::string msgPeer_; 
};