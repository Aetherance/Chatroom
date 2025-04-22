cmake_minimum_required(VERSION 3.28.3)
project(Chatroom)

add_executable(client
    UserClient.cc
    main.cc
    Client.cc
    LoginController.cc
    RegisterController.cc
)

find_package(ftxui REQUIRED COMPONENTS component dom)

target_link_libraries(client
    netlib
    jsoncpp
    ftxui::component
    ftxui::dom
    ftxui::screen
)