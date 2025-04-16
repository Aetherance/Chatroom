#ifndef SOCKETOPS_H
#define SOCKETOPS_H

#include<arpa/inet.h>
#include<unistd.h>

namespace ilib {
namespace net {
namespace socket {

    int createNonblockingOrDie();
    void bindOrDie(int sockfd,const sockaddr_in & addr);
    void listenOrDie(int sockfd);
    int accept(int sockfd,sockaddr_in * addr);
    void close(int sockfd);
    int connect(int sockfd,const sockaddr_in & addr);

    void setNonBlockAndCloseOnExec(int sockfd);
    sockaddr_in getLocalAddr(int sockfd);
    sockaddr_in getPeerAddr(int sockfd);
    
    void shutdownWrite(int sockfd);
    bool isSelfConnect(int sockfd);

}
}
}

#endif