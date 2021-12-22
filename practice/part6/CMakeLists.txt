#set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(part6)

include_directories(.)
include_directories(../../tools)

link_directories(../../tools)

aux_source_directory(. SRCS)

add_executable(part6  ${SRCS})
target_link_libraries(part6  tools)
