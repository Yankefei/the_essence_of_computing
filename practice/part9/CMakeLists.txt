#set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(part9)

include_directories(.)
include_directories(../../tools)

link_directories(../../tools)

aux_source_directory(. SRCS)

add_executable(part9  ${SRCS})
target_link_libraries(part9  tools)
