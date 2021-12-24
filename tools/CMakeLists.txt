set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(tools)

include_directories(.)

add_library(tools STATIC stream.cpp  util.cpp)

target_link_libraries(tools  pthread)


add_subdirectory(test)