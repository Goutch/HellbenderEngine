cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(HellbenderEditor CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory("HBE/")

add_executable (${PROJECT_NAME}
		main.cpp
		editor/Editor.cpp
		editor/Editor.h
		editor/Terminal.cpp
		editor/Terminal.h
		editor/SceneHierarchy.cpp
		editor/SceneHierarchy.h
		editor/MenuBar.cpp
		editor/MenuBar.h
		editor/Terminal.cpp
		editor/Terminal.h
		editor/SceneView.cpp
		editor/SceneView.h
		editor/Inspector.cpp
		editor/Inspector.h
       	editor/EditorSerializer.cpp
		editor/EditorSerializer.h
		)
target_include_directories(${PROJECT_NAME}
		PUBLIC HBE/
		PUBLIC dependencies/imgui/backends
		PUBLIC dependencies/imgui/)
target_link_libraries(${PROJECT_NAME} PUBLIC HBE)

