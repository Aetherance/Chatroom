#include<sys/timerfd.h>
#include<sys/epoll.h>
#include<vector>
#include<unordered_set>
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

  void newConn(int fd);

  void stopConn(int fd);

  void setSendMessageCallback(sendMessageCallback callback) { sendMessageCallback_ = callback; };

  void beat(const ilib::net::TcpConnectionPtr & conn);
  
private:
  int epfd_;

  std::unordered_map<int,int> heartBeatTimeTable_;

  sendMessageCallback sendMessageCallback_;

  ilib::net::TcpServer * server_;

  std::unordered_set<int> conns_; 
};

inline void ServerHeart::newConn(int fd) {
  conns_.insert(fd);
  heartBeatTimeTable_[fd] = ilib::base::Timestamp::now().secondsSinceEpoch();
}

inline void ServerHeart::stopConn(int fd) {
  conns_.erase(fd);
}

inline ServerHeart::ServerHeart(ilib::net::TcpServer * server) {
  epfd_ = ::epoll_create1(0);
  std::thread([&]{
    while (true) {
      std::vector<::epoll_event> revents(16);
      int n = ::epoll_wait(epfd_,revents.data(),1,60*1000);
      int i = 0;
      for(auto connIt = conns_.begin(); i<conns_.size() ; ++connIt,++i) {
        if(connIt != conns_.end()) {
          int conn = *connIt;
          int diff = ilib::Timestamp::now().secondsSinceEpoch() - heartBeatTimeTable_[conn];
          if(diff > 75) {
            LOG_WARN("连接超时!");
            stopConn(conn);
            ::close(conn);
          }
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