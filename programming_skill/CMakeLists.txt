set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(programming_skill)

include_directories(../tools)

link_directories(../tools)

add_executable(run_main  run_main.cpp)
target_link_libraries(run_main  tools)

add_executable(func_param_skill  func_param_skill.cpp)
target_link_libraries(func_param_skill  tools)

add_executable(align_main  align_main.cpp)
target_link_libraries(align_main  tools)


add_subdirectory(test)