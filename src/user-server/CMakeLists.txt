cmake_minimum_required(VERSION 3.28.3)
project(Chatroom)

# 添加可执行文件
add_executable(user-server
    main.cc
    UserServer.cc
    VerifiCode.cc
)

# 链接netlib库
target_link_libraries(user-server 
    netlib
    curl
    cpp_redis
    jsoncpp
)

# 包含头文件目录
target_include_directories(user-server PRIVATE
    ${CMAKE_SOURCE_DIR}/netlib/base
    ${CMAKE_SOURCE_DIR}/netlib/net
)