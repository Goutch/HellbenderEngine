cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(HellbenderEditor CXX)

include_directories("HBE/")
add_subdirectory("HBE/")

include_directories(dependencies/imgui)
add_subdirectory(dependencies/imgui)

add_executable (${PROJECT_NAME}
		main.cpp
		editor/Editor.cpp editor/Editor.h)

target_link_libraries(${PROJECT_NAME} HBE imgui)

