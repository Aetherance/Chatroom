#ifndef SOCKET_H
#define SOCKET_H

#include<arpa/inet.h>
#include<unistd.h>
#include"../base/noncopyable.h"
#include"SocketOps.h"
#include"InetAddress.h"

namespace ilib{
namespace net{

class Socket : noncopyable
{
public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {}
    ~Socket();

    int fd() const { return sockfd_; };
    
    void bind(const InetAddress& localaddr);
    void listen() const;
    int accept(InetAddress* peeraddr);

    void setReuseAddr(bool on) const;
    void shutdownWrite() const;

private:
    int sockfd_;
};

inline void Socket::bind(const InetAddress& localaddr) {
    socket::bindOrDie(sockfd_,localaddr.getSockAddr());
}

inline void Socket::listen() const {
    socket::listenOrDie(sockfd_);
}

inline int Socket::accept(InetAddress* peeraddr) {
    sockaddr_in add;
    memset(&add,0,sizeof(add));
    int sock = socket::accept(sockfd_,&add);
    if(sock >= 0) {
        *peeraddr = add;
    }
    return sock;
}

inline void Socket::setReuseAddr(bool on) const {
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
} 

inline void Socket::shutdownWrite() const {
    socket::shutdownWrite(sockfd_);
}

inline Socket::~Socket() {
    socket::close(sockfd_);
}

}
}

#endif