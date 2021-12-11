#CMake最低版本号要求
cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER /usr/local/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

#指定项目名称
project(DataStruct_And_Algorithm)

#使用默认路径的gcc制定编译期
set(CMAKE_CXX_COMPILER "g++")

#指定编译选项
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")

#指定编译目录
set(PROJECT_BINARY_DIR ./build)

#添加子目录
add_subdirectory(tools)
add_subdirectory(practice)
