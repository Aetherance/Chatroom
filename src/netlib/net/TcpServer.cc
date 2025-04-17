#include"../base/logger.h"
#include"TcpServer.h"
#include<assert.h>
using namespace ilib::net;

void TcpServer::newConnection(int sockfd,const InetAddress& peerAddr) {
    LOG_INFO("A client is going to connect to the server...");

    loop_->assertInLoopThread();
    char buff[32];
    snprintf(buff,sizeof(buff),"#%d",nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buff;

    EventLoop * subLoop = threadpool_->getNextLoop();

    InetAddress localAddr(socket::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(subLoop,connName,sockfd,localAddr,peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback([this](const TcpConnectionPtr & conn){ removeConnection(conn); });
    subLoop->runInloop([conn]{ conn->connectEstablished(); }); // ??
}

TcpServer::TcpServer(EventLoop * loop,const InetAddress & listenAddr) 
         : loop_(loop),
           acceptor_(new Acceptor(loop,listenAddr)),
           threadpool_(new EventLoopThreadPool(loop)),
           connectionCallback_(),
           messageCallback_(),
           started_(false),
           nextConnId_(1),
           name_(listenAddr.toIpPort())
{
    acceptor_->setNewConnectionCallback([this](int sockfd,const InetAddress &peerAddr){ newConnection(sockfd,peerAddr); });
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {
    if(! started_) {
        started_ = true;
        threadpool_->start();
    }

    if(! acceptor_->listenning()) {
        loop_->runInloop([this]{ acceptor_->listen(); });
    }
}

void TcpServer::removeConnection(const TcpConnectionPtr & conn) {
    loop_->runInloop([this,conn]{ removeConnectionInLoop(conn); });
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr & conn) {
    loop_->assertInLoopThread();
    size_t n = connections_.erase(conn->name());
    (void)n;
    assert(n == 1);
    EventLoop * subLoop = conn->getLoop();
    subLoop->queueInLoop([conn]{ conn->connectDestroyed(); });
}

void TcpServer::setThreadNum(int numThreads) {
    assert(numThreads >= 0);
    threadpool_->setThreadNum(numThreads);
}