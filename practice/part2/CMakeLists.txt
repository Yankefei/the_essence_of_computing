#set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(part2)

include_directories(.)
include_directories(../../tools)

aux_source_directory(. SRCS)

add_executable(part2  ${SRCS})