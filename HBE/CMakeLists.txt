cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(HBE CXX)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++17 -O3 -fPIC" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -O3 -fPIC" )

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb/")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lua/")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/")


include_directories(.)
set(CORE_FILES
        HBE.cpp
        HBE.h
        Configs.cpp

        CompilationConfigs.h
        Configs.h

        core/resource/ShaderProgram.h
        core/resource/Texture.h
        core/resource/Resource.cpp
        core/resource/Texture.cpp
        core/resource/Material.cpp
        core/resource/Material.h
        core/resource/Model.cpp
        core/resource/ShaderProgram.cpp
        core/resource/Mesh.cpp

        core/graphics/Framebuffer.h
        core/graphics/Framebuffer.cpp
        core/graphics/Renderer.h
        core/graphics/Graphics.cpp
        core/graphics/Graphics.h
        core/graphics/RenderTarget.cpp
        core/graphics/RenderTarget.h
        core/graphics/Renderer.cpp

        core/input/Input.h
        core/input/Input.cpp

        core/entity/Scene.cpp
        core/entity/Scene.h
        core/entity/Entity.cpp
        core/entity/Transform.cpp
        core/entity/component/Camera.cpp
        core/entity/component/Component.h
        core/entity/component/CameraController.cpp
        core/entity/component/MeshRenderer.cpp
        core/entity/component/MeshRenderer.h
        core/entity/component/CameraController.h
        core/entity/component/Component.cpp
        core/entity/component/InstancesRenderer.cpp
        core/entity/component/InstancesRenderer.h
        core/entity/component/ModelRenderer.h
        core/entity/component/ModelRenderer.cpp

        core/utility/Log.cpp
        core/utility/Geometry.cpp
        core/utility/Geometry.h
        core/utility/Clock.cpp
        core/utility/ModelImporter.cpp
        core/utility/JobManager.cpp
        core/utility/JobManager.h
        core/utility/Random.cpp
        core/utility/Random.h

        core/serialization/Serializer.cpp
        core/serialization/Serializer.h
        core/serialization/Deserializer.cpp
        core/serialization/Deserializer.h)

set(SCRIPTING_FILES
        scripting/ScriptManager.cpp
        scripting/ScriptManager.h
        )

set(GL_FILES
        graphics/gl/GL_Core.h
        graphics/gl/GL_Renderer.cpp
        graphics/gl/GL_Texture.cpp
        graphics/gl/GL_ShaderProgram.cpp
        graphics/gl/GL_Mesh.cpp
        graphics/gl/GL_Framebuffer.cpp
        graphics/gl/GL_Framebuffer.h
        graphics/gl/GL_Debug.cpp
        graphics/gl/GL_Debug.h
        graphics/gl/GL_MultisamplingFramebuffer.cpp
        graphics/gl/GL_MultisamplingFramebuffer.h main.cpp)

add_library(${PROJECT_NAME} SHARED
        ${CORE_FILES}
        ${GL_FILES}
        ${SCRIPTING_FILES}
        Core.h)
target_include_directories(${PROJECT_NAME}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/contrib/zlib/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/include/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/include"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lua/src/")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message("Using Clang")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message("Using GNU")
    target_link_libraries(${PROJECT_NAME}
            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/libassimp.a"
            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/contrib/zlib/libzlibstatic.a"
            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/contrib/irrXML/libIrrXML.a"
            PUBLIC glfw
            PUBLIC glad
            PUBLIC stb
            PUBLIC lua)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    error(FATAL_ERROR "Using Intel compiler is not supported")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    message("Using visual studio compiler")
    target_link_libraries(${PROJECT_NAME}
            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/assimp-vc142-mt.lib"
            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/contrib/zlib/zlibstatic.lib"
            #PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/contrib/irrXML/libIrrXML.lib"
            PUBLIC glfw
            PUBLIC glad
            PUBLIC stb
            PUBLIC lua)
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC stb)
target_link_libraries(${PROJECT_NAME} PUBLIC lua)


target_compile_definitions(${PROJECT_NAME} PRIVATE HB_BUILD)
