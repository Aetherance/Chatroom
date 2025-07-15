#include<sys/timerfd.h>
#include<sys/epoll.h>
#include<vector>
#include<functional>
#include<unistd.h>
#include<thread>
#include"logger.h"

using sendMessageCallback = std::function<void()>;

class ClientHeart
{
public:
  ClientHeart(int ConnFd);
  ~ClientHeart();
  
  void setSendMessageCallback(sendMessageCallback callback) { sendMessageCallback_ = callback; };

  void recvAck();

private:
  int fd_;
  int epfd_;

  bool recvdAck_ = true;

  sendMessageCallback sendMessageCallback_;
};

inline ClientHeart::ClientHeart(int ConnFd) {
  epfd_ = ::epoll_create1(0);
  std::thread([&]{
    while (true) {
      std::vector<::epoll_event> revents(16);
      int n = ::epoll_wait(epfd_,revents.data(),16,/* 30*1000 */15*1000);
      if(sendMessageCallback_) {
        LOG_INFO("beat");
        sendMessageCallback_();
        recvdAck_ = false;
        ::sleep(5);
        if( !recvdAck_) {
          ::close(fd_);
        }
      }
    }
  }).detach();
}

inline void ClientHeart::recvAck() {
  recvdAck_ = true;
}

inline ClientHeart::~ClientHeart() {
  ::close(epfd_);
  ::close(fd_);
}