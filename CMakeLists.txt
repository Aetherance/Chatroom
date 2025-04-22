cmake_minimum_required(VERSION 3.28.3)
project(Chatroom)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 添加子目录
add_subdirectory(src/netlib)
add_subdirectory(src/server)
add_subdirectory(src/client)