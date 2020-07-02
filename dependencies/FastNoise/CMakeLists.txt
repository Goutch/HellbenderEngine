cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(FastNoise)

add_library(${PROJECT_NAME}
        FastNoise.cpp
        )
