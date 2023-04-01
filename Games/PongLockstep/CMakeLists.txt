cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
project(PongOnline CXX)

include_directories(dependencies/fpm/include)

add_subdirectory(dependencies/enet)
include_directories(dependencies/enet/include)


add_executable(${PROJECT_NAME}
        main.cpp
        PongGame.h
        PongUIScene.cpp
        PongUIScene.h
        PongGameScene.cpp
        PongGameScene.h
        PongGame.cpp
        Systems/BallSystem.cpp
        Systems/BallSystem.h
        Systems/PaddleSystem.cpp
        Systems/PaddleSystem.h
        Simulation/fixed.h
        Simulation/Commands.h
        Simulation/PongServer.cpp
        Simulation/PongServer.h
        Simulation/Server.cpp
        Simulation/Server.h
        Simulation/Server.h
        Simulation/Client.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC HBE enet winmm ws2_32)
target_include_directories(${PROJECT_NAME}
        PUBLIC "${${CMAKE_CURRENT_SOURCE_DIR}}")

target_include_directories(${PROJECT_NAME} PUBLIC dependencies/enet/include)
target_include_directories(${PROJECT_NAME} PUBLIC dependencies/fpm/include)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
        )