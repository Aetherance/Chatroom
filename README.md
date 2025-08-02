# 介绍

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

# 使用客户端

使用客户端:

依赖 `docker`

获取 run_cli.sh

```
wget http://10.30.0.131:9090/download/run_cli.sh && chmod +x ./run_cli.sh
```

运行客户端镜像 (无镜像时自动获取)

```
./run_cli.sh [服务器 ip]
```

程序使用:

可以使用 Ctrl + C 返回上一级

关于传输文件

下载的文件会被保存至 /tmp/download

如果使用docker镜像启动客户端 需要将要传输的文件放在 /tmp/upload 然后输入文件名上传文件

# 环境

OS: `Ubuntu 25.04 x86_64`

Shell: `fish 4.0.1` 

编译器: `g++ 14.2.0`

构建工具: `CMake 3.31.6`

客户端依赖于动态库 `Jsoncpp 1.9.6` 和 `protobuf 3.21.12`

此外 项目的依赖还有 `curl` `cpp_redis` 和 `FTXUI`

# 编译 及 运行

编译

```
mkdir build && cd build
cmake ..
make -j(nproc)
```

运行服务端
```
./server
```

运行客户端
```
./client
```

# 部署

使用 `docker compose` 部署项目

在项目根目录执行 `docker compose up`

# 补充

代码量约6700行

```
      88 text files.
      88 unique files.                              
       0 files ignored.

github.com/AlDanial/cloc v 2.04  T=0.03 s (2925.6 files/s, 292863.2 lines/s)
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                             50           1169            200           4901
C/C++ Header                    35            608             78           1669
CMake                            3             20              6            158
-------------------------------------------------------------------------------
SUM:                            88           1797            284           6728
-------------------------------------------------------------------------------
```

目录结构
```
src/
├── base
│   └── responsecode.h
├── client
│   ├── client.cc
│   ├── CMakeLists.txt
│   ├── controllers
│   │   ├── createGroup.cc
│   │   ├── deleteFriend.cc
│   │   ├── deleteGroup.cc
│   │   ├── download
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
│   │   ├── prompts.cc
│   │   ├── registerController.cc
│   │   ├── setOp.cc
│   │   ├── settings.cc
│   │   └── userSetting.cc
│   ├── download
│   ├── ftp.cc
│   ├── func.cc
│   ├── include
│   │   ├── client.h
│   │   ├── ClientHeartBeat.h
│   │   ├── ftp.h
│   │   ├── msg.h
│   │   ├── sig.h
│   │   ├── threadpool.h
│   │   └── user.h
│   ├── main.cc
│   ├── msg.cc
│   ├── threadpool.cc
│   ├── upload
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
    │   ├── chat-service.cc
    │   └── msgHandler.cc
    ├── CMakeLists.txt
    ├── ftp
    │   ├── file-service.cc
    │   └── ftpServer.cc
    ├── include
    │   ├── ChatServer.h
    │   ├── DBWriterPool.h
    │   ├── download
    │   ├── FtpServer.h
    │   ├── history
    │   ├── ServerHeartBeat.h
    │   ├── ServiceHandler.h
    │   ├── upload
    │   └── UserServer.h
    ├── main.cc
    ├── service
    │   ├── friends.cc
    │   ├── groups.cc
    │   └── service.cc
    └── user
        ├── auth-service.cc
        ├── userServer.cc
        └── verifiCode.cc

21 directories, 94 files
```