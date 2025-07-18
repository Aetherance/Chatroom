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

配置客户端动态库

Jsoncpp 1.9.6
```
wget https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.9.6.tar.gz -O 1.9.6.tar.gz
tar -xzvf 1.9.6.tar.gz

cd jsoncpp-1.9.6/
mkdir build

cd build
cmake -DCMAKE_BUILD_TYPE=Release -DJSONCPP_WITH_TESTS=OFF -DBUILD_SHARED_LIBS=ON ..
make -j$(nproc)
sudo make install
```

protobuf 3.21.12

```
https://github.com/protocolbuffers/protobuf/archive/refs/tags/v3.21.12.tar.gz
tar -xzvf v3.21.12.tar.gz
./autogen.sh
./configure --prefix=/usr/local
make -j$(nproc)
sudo make install
sudo ldconfig
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

代码量约6000行

```
cloc ../src/server/ ../src/netlib/ ../src/client/
      83 text files.
      83 unique files.                              
       0 files ignored.

-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                             45           1015            139           4293
C/C++ Header                    35            577             68           1620
CMake                            3             11              6             99
-------------------------------------------------------------------------------
SUM:                            83           1603            213           6012
-------------------------------------------------------------------------------
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
│   │   ├── fileService.cc
│   │   ├── fileTrans.cc
│   │   ├── find.cc
│   │   ├── friendlist.cc
│   │   ├── friendVerify.cc
│   │   ├── grouplist.cc
│   │   ├── groupSetting.cc
│   │   ├── groupVerify.cc
│   │   ├── kickMember.cc
│   │   ├── loginController.cc
│   │   ├── msgController.cc
│   │   ├── registerController.cc
│   │   ├── setOp.cc
│   │   ├── settings.cc
│   │   └── userSetting.cc
│   ├── download
│   ├── ftp.cc
│   ├── func.cc
│   ├── include
│   │   ├── ClHeartBeat.h
│   │   ├── client.h
│   │   ├── ClientHeartBeat.h
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
└── server
    ├── chat
    │   ├── chatServer.cc
    │   └── msgHandler.cc
    ├── CMakeLists.txt
    ├── ftp
    │   └── ftpServer.cc
    ├── include
    │   ├── ChatServer.h
    │   ├── DBWriterPool.h
    │   ├── FtpServer.h
    │   ├── heartBeat.h
    │   ├── ServerHeartBeat.h
    │   ├── ServiceHandler.h
    │   └── UserServer.h
    ├── main.cc
    ├── service
    │   ├── friends.cc
    │   ├── groups.cc
    │   └── service.cc
    └── user
        ├── userServer.cc
        └── verifiCode.cc

16 directories, 90 files
```