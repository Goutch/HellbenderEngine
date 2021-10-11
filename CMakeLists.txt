cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(App CXX)
include_directories(".")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory("HBE/")
add_subdirectory("HellbenderPlayground/")


