// #ifndef CONNECTOR_H
// #define CONNECTOR_H

// #include"../base/noncopyable.h"
// #include"InetAddress.h"
// #include"EventLoop.h"
// #include"Channel.h"
// #include"TimerId.h"
// #include<functional>

// namespace ilib{
// namespace net{

// class Connector
// {
// public:
//     using NewConnectionCallback = std::function<void(int sockfd)>;
//     Connector(EventLoop * loop,const InetAddress & addr);
//     ~Connector();

//     void setNewConnectionCallback(const NewConnectionCallback & cb) {  }
// private:
//     NewConnectionCallback newConnectionCallback_;
// };

// }
// }

// #endif