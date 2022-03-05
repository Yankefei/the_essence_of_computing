set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(test)

include_directories(../../tools)
include_directories(..)

link_directories(../../tools)

add_executable(singleton_main  singleton_main.cpp)
target_link_libraries(singleton_main  tools)