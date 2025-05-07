cmake_minimum_required(VERSION 3.28.3)
project(Chatroom)

# 添加可执行文件
add_executable(server
    main.cc
    chat/chatServer.cc
    chat/msgHandler.cc
    user/userServer.cc
    user/verifiCode.cc
    ${CMAKE_SOURCE_DIR}/src/protobuf/msg.pb.cc
)

# 链接netlib库
target_link_libraries(server
    netlib
    curl
    cpp_redis
    jsoncpp
    protobuf
)

# 包含头文件目录
target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/src/base
    ${CMAKE_SOURCE_DIR}/src/server/include
    ${CMAKE_SOURCE_DIR}/src/netlib/base
    ${CMAKE_SOURCE_DIR}/src/netlib/net
    ${CMAKE_SOURCE_DIR}/src/protobuf
)