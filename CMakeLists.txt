cmake_minimum_required(VERSION 3.17)
project(canvaPaintServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(canvaPaintServer src/main.cpp src/CanvaPaintServer.cpp src/CanvaPaintServer.h)
target_include_directories(canvaPaintServer PRIVATE lib)

add_subdirectory(lib/zlib)
target_link_libraries(canvaPaintServer zlib)

add_subdirectory(lib/uSockets)
target_link_libraries(canvaPaintServer uSockets)

add_subdirectory(lib/uWebSockets)
target_link_libraries(canvaPaintServer uWebSockets)

