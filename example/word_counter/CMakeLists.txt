cmake_minimum_required(VERSION 3.10)
project(map_reduce_example CXX)

set(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE(Boost REQUIRED COMPONENTS locale)

add_library(map_reduce STATIC IMPORTED)
set_property(TARGET map_reduce PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../../lib/libmap_reduce.so)

add_executable(example main.cpp)
target_link_libraries(example map_reduce)

add_library(map_reduce_config SHARED config.cpp)
target_link_libraries(map_reduce_config ${Boost_LOCALE_LIBRARY})
