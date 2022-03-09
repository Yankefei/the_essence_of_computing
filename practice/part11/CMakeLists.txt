#set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(part11)

include_directories(.)
include_directories(../../tools)

aux_source_directory(. SRCS)

add_executable(part11  ${SRCS})
target_link_libraries(part11  tools)