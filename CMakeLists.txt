cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(App CXX)
include_directories(".")


set(HBE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/out)

add_subdirectory("HBE/")
add_subdirectory("HellbenderPlayground/")
add_subdirectory("Games/Pong/")
add_subdirectory("Games/PongLockstep/")
add_subdirectory("Games/Raytracer/")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${HBE_OUTPUT_DIR})