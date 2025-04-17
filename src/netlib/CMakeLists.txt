cmake_minimum_required(VERSION 3.28.3)
project(netlib)

# 收集源文件
set(NETLIB_SOURCES
    base/timestamp.cc
    net/Acceptor.cc
    net/Buffer.cc
    net/Channel.cc
    net/Epoller.cc
    net/EventLoop.cc
    net/EventLoopThreadPool.cc
    net/Poller.cpp
    net/SocketOps.cc
    net/TcpConnection.cc
    net/TcpServer.cc
    net/TimerQueue.cc
)

# 创建对象库
add_library(netlib OBJECT ${NETLIB_SOURCES})

# 包含头文件目录
target_include_directories(netlib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/net
)