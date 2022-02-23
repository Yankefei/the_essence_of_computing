set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(tools)

include_directories(.)

aux_source_directory(. SRCS)

add_library(tools STATIC ${SRCS})
target_link_libraries(tools  pthread)

add_subdirectory(test)
add_subdirectory(test_performance)


aux_source_directory(lock_free_container K_SRCS)
add_library(fifo_tools STATIC ${K_SRCS} stream.cpp)
target_link_libraries(fifo_tools  pthread)

add_subdirectory(test_lock_free)
