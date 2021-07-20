cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(HellbenderPlayground CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
include_directories("HBE/")
add_subdirectory("HBE/")

add_executable (${PROJECT_NAME}
		main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC HBE)

