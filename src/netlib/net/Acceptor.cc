#include"Acceptor.h"
#include"InetAddress.h"
#include"logger.h"

using namespace ilib::net;

Acceptor::Acceptor(EventLoop * loop,const InetAddress & listenAddr) 
    : loop_(loop) , acceptSocket_(socket::createNonblockingOrDie()) ,  
      acceptChannel_(loop,acceptSocket_.fd()) , listenning_ (false) {
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bind(listenAddr);
    acceptChannel_.setReadCallback([this](Timestamp){handleRead(); });
}

void Acceptor::listen() {
    loop_->assertInLoopThread();
    listenning_ = true;
    acceptSocket_.listen();

    LOG_INFO("Listening ...");

    acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
    loop_->assertInLoopThread();
    InetAddress peerAddr(0);

    int sock = acceptSocket_.accept(&peerAddr);
    if(sock >= 0) {
        if(newConnectionCallback_) {
            newConnectionCallback_(sock,peerAddr);
        } else {
            socket::close(sock);
        }
    }
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback & cb) {
    newConnectionCallback_ = cb;
}