set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(test_performance)

include_directories(../tools)

link_directories(../tools)

add_executable(spsc_queue_main  spsc_queue_main.cpp)
target_link_libraries(spsc_queue_main  tools)

