一个基于C++主从reactor模式的多线程TCP多人聊天室。

服务端

分为三个服务器 `UserServer` `ChatServer` `TcpServer` ， 每个服务器是一个独立的类，分别监听端口 `8080` `7070` `6060`。

`UserServer`是管理登陆注册等身份验证服务的服务器。负责登陆、注册、发送验证码等功能。

`ChatServer`是聊天室服务器的核心，负责处理消息的转发、好友和群的管理等功能。

`FtpServer`是一个文件上传和下载服务器，支持在Ftp被动模式下传输文件，但是没有实现完整的Ftp协议，仅保留发送文件功能。

客户端

分为三个子客户端 `UserClient` `ChatClient` `TcpClient` ，并由主客户端类Client管理。 每个子客户端类分别连接至对应的服务器。

客户端基于`FTXUI`库实现了简单的图形化界面，使聊天室客户端的使用更加方便。

客户端主要有两个线程，主线程负责向服务器发送请求，另一个线程运行一个EventLoop处理服务器的响应消息。

聊天室使用了`Json` 和 `protobuf`作为序列化和反序列化的方法在服务器和客户端之间通信。 服务器和客户端均实现了Tcp粘包的处理。

客户端没有使用网络库.

环境配置: (ubuntu)
```
sudo ./install_env.sh
```

编译
```
mkdir build && cd build
cmake ..
make
```

运行服务端
```
./server
```

运行客户端
```
./client
```

目录结构
```
../src/
├── base
│   └── responsecode.h
├── client
│   ├── client.cc
│   ├── CMakeLists.txt
│   ├── controllers
│   │   ├── createGroup.cc
│   │   ├── deleteFriend.cc
│   │   ├── deleteGroup.cc
│   │   ├── friendlist.cc
│   │   ├── friendVerify.cc
│   │   ├── grouplist.cc
│   │   ├── groupVerify.cc
│   │   ├── loginController.cc
│   │   ├── msgController.cc
│   │   ├── registerController.cc
│   │   └── settings.cc
│   ├── ftp.cc
│   ├── func.cc
│   ├── include
│   │   ├── client.h
│   │   ├── ftp.h
│   │   ├── msg.h
│   │   ├── sig.h
│   │   └── user.h
│   ├── main.cc
│   ├── msg.cc
│   └── user.cc
├── netlib
│   ├── base
│   │   ├── logger.h
│   │   ├── noncopyable.h
│   │   ├── timestamp.cc
│   │   └── Timestamp.h
│   ├── CMakeLists.txt
│   └── net
│       ├── Acceptor.cc
│       ├── Acceptor.h
│       ├── Buffer.cc
│       ├── Buffer.h
│       ├── Channel.cc
│       ├── Channel.h
│       ├── Connector.h
│       ├── Epoller.cc
│       ├── Epoller.h
│       ├── EventLoop.cc
│       ├── EventLoop.h
│       ├── EventLoopThread.h
│       ├── EventLoopThreadPool.cc
│       ├── EventLoopThreadPool.h
│       ├── InetAddress.h
│       ├── Poller.cpp
│       ├── Poller.h
│       ├── sigpipe.h
│       ├── Socket.h
│       ├── SocketOps.cc
│       ├── SocketOps.h
│       ├── TcpConnection.cc
│       ├── TcpConnection.h
│       ├── TcpServer.cc
│       ├── TcpServer.h
│       ├── Timer.h
│       ├── TimerId.h
│       ├── TimerQueue.cc
│       └── TimerQueue.h
├── protobuf
│   ├── fileInfo.pb.cc
│   ├── fileInfo.pb.h
│   ├── fileInfo.proto
│   ├── msg.pb.cc
│   ├── msg.pb.h
│   └── msg.proto
├── root
└── server
    ├── chat
    │   ├── chatServer.cc
    │   ├── msgHandler.cc
    │   └── service.cc
    ├── CMakeLists.txt
    ├── ftp
    │   └── ftpServer.cc
    ├── include
    │   ├── ChatServer.h
    │   ├── DBWriterPool.h
    │   ├── FtpServer.h
    │   ├── ServiceHandler.h
    │   └── UserServer.h
    ├── main.cc
    └── user
        ├── userServer.cc
        └── verifiCode.cc

15 directories, 77 files
```