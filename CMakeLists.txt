cmake_minimum_required(VERSION 3.15)
project(map_reduce)

set(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE(libssh REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(ssh)

add_library(map_reduce_config SHARED configurator/JobConfig.cpp)

add_executable(map_reduce main.cpp ssh/Scp.cpp ssh/Node.cpp types/KeyValueTypeFactory.cpp)
add_executable(map_node map_node/map_node.cpp types/KeyValueTypeFactory.cpp)
add_executable(reduce_node json_server.h types/KeyValueTypeFactory.cpp reduce_node/reduce_node.cpp)

target_link_libraries(map_reduce ${CMAKE_THREAD_LIBS_INIT} ${Boost_SYSTEM_LIBRARIES} ${Boost_FILESYSTEM_LIBRARIES})
target_link_libraries(map_node ${CMAKE_THREAD_LIBS_INIT} ${Boost_SYSTEM_LIBRARIES} ${Boost_FILESYSTEM_LIBRARIES})
target_link_libraries(reduce_node ${CMAKE_THREAD_LIBS_INIT} ${Boost_SYSTEM_LIBRARIES} ${Boost_FILESYSTEM_LIBRARIES})

target_link_libraries(map_reduce -lssh stdc++fs -ldl)
target_link_libraries(map_node -lssh stdc++fs -ldl)
target_link_libraries(reduce_node -lssh stdc++fs -ldl)
