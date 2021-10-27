cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(HellbenderPlayground CXX)

add_executable (${PROJECT_NAME}
        main.cpp src/Game.cpp src/Game.h)
target_link_libraries(${PROJECT_NAME} PUBLIC HBE)
target_include_directories(${PROJECT_NAME}
        PUBLIC "${${CMAKE_CURRENT_SOURCE_DIR}}")

