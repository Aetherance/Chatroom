#include"SocketOps.h"
#include"../base/logger.h"
#include<fcntl.h>
using namespace ilib::net;

int socket::createNonblockingOrDie() {
    int sockfd = ::socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,IPPROTO_TCP);
    if(sockfd < 0) {
        LOG_FATAL("SocketOps: Create sockfd failed!");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void socket::bindOrDie(int sockfd, const sockaddr_in & addr) {
    int stat = ::bind(sockfd,reinterpret_cast<const sockaddr*>(&addr),sizeof(addr));
    if(stat < 0) {
        LOG_FATAL("SocketOps: Bind failed!");
        exit(-1);
    }
}

void socket::listenOrDie(int sockfd) {
    int stat = ::listen(sockfd,SOMAXCONN);
    if(stat < 0) {
        LOG_FATAL("SocketOps: Listen falied!");
    }
}

int socket::accept(int sockfd,sockaddr_in * addr) {
    socklen_t len = sizeof(*addr);
    int sock = ::accept4(sockfd,reinterpret_cast<sockaddr*>(addr),&len,SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(sock < 0) {
        LOG_FATAL("SocketOps: Accept failed!");
    }
    return sock;
}

void socket::close(int sockfd) {
    int stat = ::close(sockfd);
    if(stat < 0) {
        LOG_FATAL("SocketOps: Close failed!");
    }
}

int socket::connect(int sockfd,const sockaddr_in & addr) {
    return ::connect(sockfd,reinterpret_cast<const sockaddr*>(&addr),sizeof(addr));
}

void socket::setNonBlockAndCloseOnExec(int sockfd) {
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);

    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);
}

sockaddr_in socket::getLocalAddr(int sockfd) {
    sockaddr_in sin;
    memset(&sin,0,sizeof(sockaddr_in));
    socklen_t sin_len = sizeof(sin);
    if(::getsockname(sockfd,reinterpret_cast<sockaddr*>(&sin),&sin_len) < 0) {
        LOG_FATAL("SocketOps: Get sock name failed!");
    }
    return sin;
}

sockaddr_in socket::getPeerAddr(int sockfd) {
    sockaddr_in sin;
    memset(&sin,0,sizeof(sockaddr_in));
    socklen_t sin_len = sizeof(sin);
    if(::getpeername(sockfd,reinterpret_cast<sockaddr*>(&sin),&sin_len) < 0) {
        LOG_FATAL("SocketOps: Get sock name failed!");
    }
    return sin;
}

void socket::shutdownWrite(int sockfd) {
    if(::shutdown(sockfd,SHUT_WR) < 0) {
        LOG_ERROR("SocketOps: ShutdownWrite Error");
    }
}

bool socket::isSelfConnect(int sockfd) {
    sockaddr_in local = getLocalAddr(sockfd);
    sockaddr_in peer = getPeerAddr(sockfd);
    return (local.sin_port == peer.sin_port) && (local.sin_addr.s_addr == peer.sin_addr.s_addr);
}