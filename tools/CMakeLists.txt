set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(tools)

include_directories(.)

aux_source_directory(. SRCS)

#add_executable(part9  ${SRCS})
add_library(tools STATIC ${SRCS})

target_link_libraries(tools  pthread)


add_subdirectory(test)