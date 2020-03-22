cmake_minimum_required(VERSION 3.15)
project(map_reduce)

set(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE(libssh REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(SSH_Node)

add_library(map_reduce_config SHARED configurator/JobConfig.cpp)

add_executable(map_reduce main.cpp SSH_Node/Scp.cpp SSH_Node/Node.cpp types/KeyValueTypeFactory.cpp)
add_executable(map_server map_server.cpp types/KeyValueTypeFactory.cpp)
add_executable(reduce_server reduce_server.cpp types/KeyValueTypeFactory.cpp)

target_link_libraries(map_reduce ${CMAKE_THREAD_LIBS_INIT} ${Boost_SYSTEM_LIBRARIES} ${Boost_FILESYSTEM_LIBRARIES})
target_link_libraries(map_server ${CMAKE_THREAD_LIBS_INIT} ${Boost_SYSTEM_LIBRARIES} ${Boost_FILESYSTEM_LIBRARIES})
target_link_libraries(reduce_server ${CMAKE_THREAD_LIBS_INIT} ${Boost_SYSTEM_LIBRARIES} ${Boost_FILESYSTEM_LIBRARIES})

target_link_libraries(map_reduce -lssh stdc++fs -ldl)
target_link_libraries(map_server -lssh stdc++fs -ldl)
target_link_libraries(reduce_server -lssh stdc++fs -ldl)
