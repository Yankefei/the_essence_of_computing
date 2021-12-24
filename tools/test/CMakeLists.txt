set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(test)

include_directories(../tools)

link_directories(../tools)

add_executable(queue_main  queue_main.cpp)
target_link_libraries(queue_main  tools)

add_executable(tree_main  tree_main.cpp)
target_link_libraries(tree_main  tools)

add_executable(stack_main  stack_main.cpp)
target_link_libraries(stack_main  tools)

add_executable(vector_main  vector_main.cpp)
target_link_libraries(vector_main  tools)