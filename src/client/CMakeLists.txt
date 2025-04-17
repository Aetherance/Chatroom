cmake_minimum_required(VERSION 3.28.3)
project(Chatroom)

add_executable(client
    buff.cc
    client.cc
    main.cc
)

target_link_libraries(client
    netlib
)
