cmake_minimum_required(VERSION 3.15)
project(map_reduce)

set(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE(libssh REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(SSH_Node)

add_executable(map_reduce main.cpp SSH_Node/Scp.cpp SSH_Node/Node.cpp ValueType.cpp)
add_library(map_reduce_configurator_library SHARED JobConf.cpp)
target_link_libraries(map_reduce ${CMAKE_THREAD_LIBS_INIT} ${Boost_SYSTEM_LIBRARIES} ${Boost_FILESYSTEM_LIBRARIES})

target_link_libraries(map_reduce -lssh stdc++fs -ldl)
