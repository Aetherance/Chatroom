#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include"../base/noncopyable.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include"Socket.h"
#include"Buffer.h"

namespace ilib{
namespace net{

class Buffer;
class TcpConnection : noncopyable , public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
    using MessageCallback = std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)>;
    using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
    using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

    TcpConnection(EventLoop * loop,std::string connName,int sockfd,InetAddress localAddr,InetAddress peerAddr);

    EventLoop * getLoop() const { return loop_; };

    const std::string & name() const { return name_; };
    const InetAddress & localAddress() const { return localAddr_; }
    const InetAddress & peerAddress() const { return peerAddr_; }
    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }

    void setConnectionCallback(ConnectionCallback cb) { connectionCallback_ = cb; }
    void setMessageCallback(MessageCallback cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(WriteCompleteCallback cb) { writeCompleteCallback_ = cb; }
    void setCloseCallback(CloseCallback cb) { closeCallback_ = cb; }

    void connectEstablished();
    void connectDestroyed();

    void send(const std::string& message);

    void shutdown();
    int fd() const { return channel_->fd(); }

    void set_user_email(const std::string & email) { user_email_ = email; }

    std::string user_email() const { return user_email_; }

private:
    enum StateE { kConnecting, kConnected , kDisconnecting , kDisconnected };

    void setState(StateE s) { state_ = s; };
    void handleRead(Timestamp receiveTime);
    void handleClose();
    void handleWrite();
    void handleError();
    void sendInLoop(const std::string msg);
    void shutdownInLoop();

    EventLoop * loop_;
    std::string name_;
    StateE state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback closeCallback_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;

    std::string user_email_;
};

}
}


#endif