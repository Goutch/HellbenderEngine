cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(HellbenderPlayground CXX)

add_executable(${PROJECT_NAME}
        main.cpp
        Raytracer/TracedRendererSystem.cpp
        Pathfinder/AgentSystem.cpp
        Pathfinder/AgentSystem.h
        Pathfinder/SimTransform.h
        Pathfinder/SimTransformSystem.cpp
        Pathfinder/PlayerInputSystem.cpp
        Pathfinder/PlayerInputSystem.h)
target_link_libraries(${PROJECT_NAME} PUBLIC HBE)
target_include_directories(${PROJECT_NAME}
        PUBLIC "${${CMAKE_CURRENT_SOURCE_DIR}}")

