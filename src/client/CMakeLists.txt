cmake_minimum_required(VERSION 3.28.3)
project(Chatroom)

add_executable(client
    UserClient.cc
    main.cc
)

target_link_libraries(client
    netlib
    jsoncpp
)