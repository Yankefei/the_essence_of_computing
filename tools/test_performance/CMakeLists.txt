set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(test_performance)

include_directories(../tools)

link_directories(../tools)

add_executable(hash_map_performance  hash_map_performance.cpp)
target_link_libraries(hash_map_performance  tools)

add_executable(map_performance  map_performance.cpp)
target_link_libraries(map_performance  tools)
