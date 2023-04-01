cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(Pong CXX)
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
        FPSCounter.h)

target_link_libraries(${PROJECT_NAME} PUBLIC HBE)
target_include_directories(${PROJECT_NAME}
        PUBLIC "${${CMAKE_CURRENT_SOURCE_DIR}}")

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
        )