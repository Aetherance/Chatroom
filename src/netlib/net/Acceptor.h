#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include<functional>
#include"EventLoop.h"
#include"InetAddress.h"
#include"Socket.h"

namespace ilib{
namespace net{
class Acceptor
{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;

    Acceptor(EventLoop * loop,const InetAddress& listenAddr);
    void setNewConnectionCallback(const NewConnectionCallback & cb);
    bool listenning() const { return listenning_; };
    void listen();
private:
    void handleRead();

    EventLoop * loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};

}
}

#endif
