set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(test_performance)

include_directories(../tools)

link_directories(../tools)

set(CMAKE_CXX_FLAGS "-latomic")

add_executable(spsc_queue_main  spsc_queue_main.cpp)
target_link_libraries(spsc_queue_main  tools)

add_executable(test_fifo_struct_main test_fifo_struct_main.cpp)
target_link_libraries(test_fifo_struct_main  fifo_tools)

add_executable(test_atomic_main  test_atomic_main.cpp)
target_link_libraries(test_atomic_main  tools)
