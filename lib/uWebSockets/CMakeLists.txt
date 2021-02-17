cmake_minimum_required(VERSION 3.17)
project(uWebSockets)

set(CMAKE_CXX_STANDARD 20)

add_library(uWebSockets App.h)
set_target_properties(uWebSockets PROPERTIES LINKER_LANGUAGE CXX)

