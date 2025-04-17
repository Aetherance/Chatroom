#ifndef TCPSERVER_H
#define TCPSERVER_H

#include"../base/noncopyable.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include<map>
#include"Acceptor.h"
#include"TcpConnection.h"
#include"EventLoopThreadPool.h"

namespace ilib{
namespace net{

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
class TcpServer : noncopyable
{
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using ConnectionMap = std::map<std::string,TcpConnectionPtr>;
using MessageCallback = std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
    
public:
    TcpServer(EventLoop * loop,const InetAddress & listenAddr);
    ~TcpServer();

    void start();

    void setThreadNum(int numThreads);
    void setConnectionCallback(const ConnectionCallback & cb) { connectionCallback_ = cb; };
    void setMessageCallback(const MessageCallback & cb) { messageCallback_ = cb; };
private:
    void newConnection(int sockfd,const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr & conn);
    void removeConnectionInLoop(const TcpConnectionPtr & conn);

    EventLoop * loop_;
    const std::string name_;
    
    std::unique_ptr<Acceptor>acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadpool_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    bool started_;
    int nextConnId_;
    ConnectionMap connections_;
};

}
}

#endif