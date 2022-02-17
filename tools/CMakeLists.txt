set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(tools)

include_directories(.)

aux_source_directory(. SRCS)

add_library(tools STATIC ${SRCS})

target_link_libraries(tools  pthread)


add_subdirectory(test)
add_subdirectory(test_performance)
add_subdirectory(test_lock_free)
