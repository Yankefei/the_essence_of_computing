set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(test)

include_directories(../tools)

link_directories(../tools)

add_executable(queue_main  queue_main.cpp)
target_link_libraries(queue_main  tools)

add_executable(binary_tree_main  binary_tree_main.cpp)
target_link_libraries(binary_tree_main  tools)

add_executable(stack_main  stack_main.cpp)
target_link_libraries(stack_main  tools)

add_executable(vector_main  vector_main.cpp)
target_link_libraries(vector_main  tools)

add_executable(binary_tree2_main  binary_tree2_main.cpp)
target_link_libraries(binary_tree2_main  tools)

add_executable(string_main  string_main.cpp)
target_link_libraries(string_main  tools)

add_executable(list_main  list_main.cpp)
target_link_libraries(list_main  tools)

add_executable(bt_tree_main  bt_tree_main.cpp)
target_link_libraries(bt_tree_main  tools)

add_executable(bs_tree_main  bs_tree_main.cpp)
target_link_libraries(bs_tree_main  tools)

add_executable(deque_main  deque_main.cpp)
target_link_libraries(deque_main  tools)

add_executable(avl_tree_main  avl_tree_main.cpp)
target_link_libraries(avl_tree_main  tools)

add_executable(avl_tree_main2  avl_tree_main2.cpp)
target_link_libraries(avl_tree_main2  tools)

add_executable(avl_tree_main3  avl_tree_main3.cpp)
target_link_libraries(avl_tree_main3  tools)

add_executable(avl_tree_check avl_tree_check_main.cpp)
target_link_libraries(avl_tree_check  tools)


add_executable(rb_tree_main rb_tree_main.cpp)
target_link_libraries(rb_tree_main  tools)