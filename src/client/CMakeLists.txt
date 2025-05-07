cmake_minimum_required(VERSION 3.28.3)
project(Chatroom)

add_executable(client
    user.cc
    main.cc
    client.cc
    loginController.cc
    registerController.cc
    ${CMAKE_SOURCE_DIR}/src/protobuf/msg.pb.cc
    msg.cc
    friendlist.cc
    msgController.cc
)

find_package(ftxui REQUIRED COMPONENTS component dom)

target_link_libraries(client
    netlib
    jsoncpp
    ftxui::component
    ftxui::dom
    ftxui::screen
    protobuf
)

target_include_directories(client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/base
    ${CMAKE_SOURCE_DIR}/src/protobuf
)