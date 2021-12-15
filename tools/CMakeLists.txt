set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(tools)

include_directories(.)

aux_source_directory (. SRCS)

add_library(tools STATIC ${SRCS})

target_link_libraries(tools  pthread)



add_executable(tools_main  main.cpp)
target_link_libraries(tools_main  tools)