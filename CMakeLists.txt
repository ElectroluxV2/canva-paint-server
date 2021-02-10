cmake_minimum_required(VERSION 3.17)
project(canvaPaintServer)

set(CMAKE_CXX_STANDARD 20)
set(CONAN_CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(canvaPaintServer src/main.cpp src/CanvaPaintServer.cpp src/CanvaPaintServer.h)
target_link_libraries(canvaPaintServer ${CONAN_LIBS})