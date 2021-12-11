#set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(part1)

include_directories(.)
include_directories(../../tools)

add_executable(find_max_subinterval  main.cpp find_max_subinterval_1.cpp)