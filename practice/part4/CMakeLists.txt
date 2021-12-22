#set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(part4)

include_directories(.)
include_directories(../../tools)

aux_source_directory(. SRCS)

add_executable(part4  ${SRCS})