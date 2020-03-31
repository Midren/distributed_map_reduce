cmake_minimum_required(VERSION 3.10)
project(map_reduce_example CXX)

set(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE(libssh REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system filesystem program_options REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

add_library(map_reduce STATIC IMPORTED)

set_property(TARGET map_reduce PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../lib/libmap_reduce.a)

add_library(map_reduce_config SHARED config.cpp)
add_executable(example main.cpp)
target_link_libraries(example map_reduce ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} stdc++fs -ldl -lssh)
