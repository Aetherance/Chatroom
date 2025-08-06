cmake_minimum_required(VERSION 3.28.3)
project(Chatroom)

# 添加可执行文件
add_executable(server
    main.cc
    chat/chatServer.cc
    chat/msgHandler.cc
    chat/threadpool.cc
    user/userServer.cc
    user/verifiCode.cc
    ftp/ftpServer.cc
    ${CMAKE_SOURCE_DIR}/src/protobuf/msg.pb.cc
    ${CMAKE_SOURCE_DIR}/src/protobuf/fileInfo.pb.cc
    service/service.cc
    service/friends.cc
    service/groups.cc
)

target_link_libraries(server
    netlib
    curl
    cpp_redis
    jsoncpp
    protobuf
    profiler
)

# 包含头文件目录
target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/src/base
    ${CMAKE_SOURCE_DIR}/src/server/include
    ${CMAKE_SOURCE_DIR}/src/netlib/base
    ${CMAKE_SOURCE_DIR}/src/netlib/net
    ${CMAKE_SOURCE_DIR}/src/protobuf
)

# add_executable(chat-service
#     chat/chat-service.cc
#     chat/chatServer.cc
#     chat/msgHandler.cc
#     ${CMAKE_SOURCE_DIR}/src/protobuf/msg.pb.cc
#     service/service.cc
#     service/friends.cc
#     service/groups.cc
#     chat/threadpool.cc
# )

# target_link_libraries(chat-service
#     netlib
#     cpp_redis
#     jsoncpp
#     protobuf
# )

# target_include_directories(chat-service PRIVATE
#     ${CMAKE_SOURCE_DIR}/src/base
#     ${CMAKE_SOURCE_DIR}/src/server/include
#     ${CMAKE_SOURCE_DIR}/src/netlib/base
#     ${CMAKE_SOURCE_DIR}/src/netlib/net
#     ${CMAKE_SOURCE_DIR}/src/protobuf
# )

# add_executable(auth-service
#     user/auth-service.cc
#     user/userServer.cc
#     user/verifiCode.cc
# )

# target_link_libraries(auth-service
#     netlib
#     cpp_redis
#     jsoncpp
#     curl
# )

# target_include_directories(auth-service PRIVATE
#     ${CMAKE_SOURCE_DIR}/src/base
#     ${CMAKE_SOURCE_DIR}/src/server/include
#     ${CMAKE_SOURCE_DIR}/src/netlib/base
#     ${CMAKE_SOURCE_DIR}/src/netlib/net
#     ${CMAKE_SOURCE_DIR}/src/protobuf
# )

# add_executable(file-service
#     ftp/file-service.cc
#     ftp/ftpServer.cc
#     ${CMAKE_SOURCE_DIR}/src/protobuf/fileInfo.pb.cc
# )

# target_link_libraries(file-service
#     netlib
#     cpp_redis
#     protobuf
# )

# target_include_directories(file-service PRIVATE
#     ${CMAKE_SOURCE_DIR}/src/base
#     ${CMAKE_SOURCE_DIR}/src/server/include
#     ${CMAKE_SOURCE_DIR}/src/netlib/base
#     ${CMAKE_SOURCE_DIR}/src/netlib/net
#     ${CMAKE_SOURCE_DIR}/src/protobuf
# )