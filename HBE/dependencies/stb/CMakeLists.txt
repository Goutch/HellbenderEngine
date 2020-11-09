cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(stb)

add_library(${PROJECT_NAME} STATIC
        stb_image.cpp
        )
