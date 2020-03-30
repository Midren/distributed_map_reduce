cmake_minimum_required(VERSION 3.15)
project(map_reduce CXX)

set(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE(libssh REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system filesystem program_options REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(ssh)

add_library(map_reduce_config SHARED configurator/JobConfig.cpp)

add_executable(map_reduce main.cpp ssh/Scp.cpp ssh/Node.cpp types/KeyValueTypeFactory.cpp)
add_executable(map_node map_node/map_node.cpp types/KeyValueTypeFactory.cpp)
add_executable(reduce_node reduce_node/json_server.h types/KeyValueTypeFactory.cpp reduce_node/reduce_node.cpp)

target_link_libraries(map_reduce ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
target_link_libraries(map_node ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
target_link_libraries(reduce_node ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

target_link_libraries(map_reduce -lssh stdc++fs -ldl)
target_link_libraries(map_node stdc++fs -ldl)
target_link_libraries(reduce_node stdc++fs -ldl)

if (UNIX AND NOT APPLE AND NOT CYGWIN)
    include(${PROJECT_SOURCE_DIR}/PVS-Studio.cmake)
        pvs_studio_add_target(TARGET map_reduce.analyze ALL
                OUTPUT FORMAT errorfile
                ANALYZE map_reduce
                MODE GA:1,2 OP
                LOG target1.err)
    #        pvs_studio_add_target(TARGET reduce_node.analyze ALL
    #                OUTPUT FORMAT errorfile
    #                ANALYZE reduce_node
    #                MODE GA:1,2 OP
    #                LOG target2.err)
    #        pvs_studio_add_target(TARGET map_node.analyze ALL
    #                OUTPUT FORMAT errorfile
    #                ANALYZE map_node
    #                MODE GA:1,2 OP
    #                LOG target.err)
    #    pvs_studio_add_target(TARGET map_channelreduce_config.analyze ALL
    #            OUTPUT FORMAT errorfile
    #            ANALYZE map_reduce_config
    #            MODE GA:1,2 OP
    #            LOG target1.err)
endif ()