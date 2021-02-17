cmake_minimum_required(VERSION 3.17)
project(uSockets)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC "*.c" "*.cpp")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_library(uSockets ${SRC})

find_package (Threads REQUIRED)

target_link_libraries(uSockets ${CMAKE_THREAD_LIBS_INIT})

target_compile_definitions(uSockets PUBLIC LIBUS_NO_SSL)

target_include_directories(uSockets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")