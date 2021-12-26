set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(just_run)

include_directories(../tools)

link_directories(../tools)

add_executable(run_main  run_main.cpp)
target_link_libraries(run_main  tools)

