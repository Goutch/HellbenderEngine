cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(HellbenderEditor CXX)

include_directories("HBE/")
add_subdirectory("HBE/")

include_directories(dependencies/imgui)
set(CORE_FILES)

add_executable (${PROJECT_NAME}
		main.cpp
		)

target_link_libraries(${PROJECT_NAME} HBE imgui)

