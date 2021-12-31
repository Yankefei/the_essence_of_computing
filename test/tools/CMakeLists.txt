
# queue
include_directories(../../tools)
include_directories(../../3rd/g_test/include)

link_directories(../../tools)
link_directories(../../3rd/g_test/lib)

add_executable(queue_test queue_test.cpp)
target_link_libraries(queue_test  tools)
target_link_libraries(queue_test  gtest gtest_main)  # 使用gtest的main函数

add_executable(string_test string_test.cpp)
target_link_libraries(string_test  tools)
target_link_libraries(string_test  gtest gtest_main)  # 使用gtest的main函数
