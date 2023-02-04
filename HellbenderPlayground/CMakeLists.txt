cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(HellbenderPlayground CXX)

set(PATHFINDER_FILES
        Pathfinder/Systems/AgentSystem.cpp
        Pathfinder/Systems/AgentSystem.h
        Pathfinder/Systems/SimTransformSystem.h
        Pathfinder/Systems/SimTransformSystem.cpp
        Pathfinder/Systems/PlayerInputSystem.cpp
        Pathfinder/Systems/PlayerInputSystem.h
        Pathfinder/Navmesh.cpp
        Pathfinder/Navmesh.h Pathfinder/Map.cpp
        Pathfinder/Map.h
        Pathfinder/Systems/SimCamera2DSystem.cpp
        Pathfinder/Systems/SimCamera2DSystem.h)

set(VOXEL_MODULE_FILES
        VoxelModule/VoxelRendererSystem.cpp
        )


set(EXEMPLES_FILES
        VoxelModule/VoxTree.h
        Examples/ModelScene.h
        Examples/RaytracingScene.h
        Examples/TextScene.h
        Examples/CubeScene.h
        Examples/FPSCounter.h
        Examples/TriangleScene.h Examples/OrderedRenderingScene.h)

add_executable(${PROJECT_NAME}
        main.cpp
        ${EXEMPLES_FILES}
        ${PATHFINDER_FILES}
        ${VOXEL_MODULE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC HBE)
target_include_directories(${PROJECT_NAME}
        PUBLIC "${${CMAKE_CURRENT_SOURCE_DIR}}")




