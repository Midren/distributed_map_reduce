cmake_minimum_required(VERSION 3.10)
project(map_reduce CXX)

set(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE(libssh REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system filesystem program_options REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LIBSSH_INCLUDE_DIRS})

add_library(map_reduce STATIC map_reduce.cpp configurator/config.cpp types/KeyValueTypeFactory.cpp ssh/Node.cpp ssh/Scp.cpp util.cpp)
target_link_libraries(map_reduce PUBLIC ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} stdc++fs -ldl -lssh)

add_executable(map_node map_node/map_node.cpp types/KeyValueTypeFactory.cpp)
target_link_libraries(map_node ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} map_reduce)

add_executable(reduce_node reduce_node/json_server.h types/KeyValueTypeFactory.cpp reduce_node/reduce_node.cpp reduce_node/reduce.cpp)
target_link_libraries(reduce_node ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} map_reduce)

install(TARGETS map_reduce DESTINATION ${CMAKE_SOURCE_DIR}/lib)
install(TARGETS map_node reduce_node DESTINATION ${CMAKE_SOURCE_DIR}/bin)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG" AND UNIX AND NOT APPLE AND NOT CYGWIN)
    include(${PROJECT_SOURCE_DIR}/PVS-Studio.cmake)
    pvs_studio_add_target(TARGET reduce_node.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE reduce_node
            MODE GA:1,2 OP
            LOG target2.err)
    #        pvs_studio_add_target(TARGET map_node.analyze ALL
    #                OUTPUT FORMAT errorfile
    #                ANALYZE map_node
    #                MODE GA:1,2 OP
    #                LOG target.err)
endif ()