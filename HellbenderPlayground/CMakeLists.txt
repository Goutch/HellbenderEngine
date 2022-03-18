cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(HellbenderPlayground CXX)

add_executable(${PROJECT_NAME}
        main.cpp
        Raytracer/TracedRendererSystem.cpp
        Pathfinder/Systems/AgentSystem.cpp
        Pathfinder/Systems/AgentSystem.h
        Pathfinder/Systems/SimTransformSystem.h
        Pathfinder/Systems/SimTransformSystem.cpp
        Pathfinder/Systems/PlayerInputSystem.cpp
        Pathfinder/Systems/PlayerInputSystem.h
        Pathfinder/Navmesh.cpp
        Pathfinder/Navmesh.h Pathfinder/Map.cpp
        Pathfinder/Map.h
        Pathfinder/Systems/SimCamera2DSystem.cpp Pathfinder/Systems/SimCamera2DSystem.h)
target_link_libraries(${PROJECT_NAME} PUBLIC HBE)
target_include_directories(${PROJECT_NAME}
        PUBLIC "${${CMAKE_CURRENT_SOURCE_DIR}}")

