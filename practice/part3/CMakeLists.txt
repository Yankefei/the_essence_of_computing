#set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(part3)

include_directories(.)
include_directories(../../tools)

aux_source_directory(. SRCS)

add_executable(part3  ${SRCS})
target_link_libraries(part3  tools)