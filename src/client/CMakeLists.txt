cmake_minimum_required(VERSION 3.28.3)
project(Chatroom)

add_executable(client
    user.cc
    main.cc
    client.cc
    msg.cc
    func.cc
    controllers/loginController.cc
    controllers/registerController.cc
    controllers/friendlist.cc
    controllers/msgController.cc
    controllers/friendVerify.cc
    controllers/deleteFriend.cc
    controllers/grouplist.cc
    controllers/createGroup.cc
    controllers/deleteGroup.cc
    controllers/groupVerify.cc
    controllers/settings.cc
    ${CMAKE_SOURCE_DIR}/src/protobuf/msg.pb.cc
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