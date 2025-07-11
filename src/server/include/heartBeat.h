#include<sys/timerfd.h>
#include<sys/epoll.h>
#include<vector>
#include<unordered_map>
#include<Timestamp.h>
#include<functional>
#include<unistd.h>
#include"logger.h"
#include"TcpServer.h"

using sendMessageCallback = std::function<void(const ilib::net::TcpConnectionPtr &)>;

#define TIME_OUT 75

class ServerHeart
{
public:
  ServerHeart(ilib::net::TcpServer * server);
  ~ServerHeart();
  
  void setSendMessageCallback(sendMessageCallback callback) {  };

  void beat(const ilib::net::TcpConnectionPtr & conn);
  
private:
  int epfd_;

  std::unordered_map<int,int> heartBeatTimeTable_;

  sendMessageCallback sendMessageCallback_;

  ilib::net::TcpServer * server_;

  ilib::net::ConnectionMap & conns_;
};

inline ServerHeart::ServerHeart(ilib::net::TcpServer * server) : conns_(server->connections_) {
  std::thread([&]{
    while (true) {
      std::vector<::epoll_event> revents(16);
      int n = ::epoll_wait(epfd_,revents.data(),1,/*60*1000*/ -1);
      for(auto & connInfo : conns_) {
        auto conn = connInfo.second;
        int now_time = ilib::base::Timestamp::now().secondsSinceEpoch();
        int diff = now_time - heartBeatTimeTable_[conn->fd()];
        if(diff > TIME_OUT) {
          conn->shutdown();
        }
      }
    }
  }).detach();
}

inline ServerHeart::~ServerHeart() {
  ::close(epfd_);
}

inline void ServerHeart::beat(const ilib::net::TcpConnectionPtr & conn) {
  heartBeatTimeTable_[conn->fd()] = ilib::base::Timestamp::now().secondsSinceEpoch();
  sendMessageCallback_(conn);
  LOG_CLIENT_INFO("receive client heart beat.",conn->fd());
}