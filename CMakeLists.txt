cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(App CXX)
include_directories(".")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(CMAKE_BUILD_TYPE EQUAL "RELEASE")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(CMAKE_BUILD_TYPE EQUAL "RELEASE")
add_subdirectory("HBE/")
add_subdirectory("HellbenderPlayground/")
add_subdirectory("Games/Pong/")
add_subdirectory("Games/PongLockstep/")
add_subdirectory("Games/Raytracer/")


