#set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(part1)

include_directories(.)
include_directories(../../tools)

aux_source_directory(. SRCS)

add_executable(1_e_1_3_find_max_subinterval  ${SRCS})