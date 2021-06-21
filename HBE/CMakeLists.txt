cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(HBE CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --std=c++17 -O3 -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -fPIC")
endif ()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/)
find_package(Vulkan)
if (VULKAN_FOUND)
    include_directories(${Vulkan_INCLUDE_DIRS})
    set(VK_FILES
            platforms/vk/VK_Renderer.cpp
            platforms/vk/VK_Instance.cpp
            platforms/vk/VK_Instance.h
            platforms/vk/VK_Window.cpp
            platforms/vk/VK_Window.h
            platforms/vk/VK_ValidationLayers.cpp
            platforms/vk/VK_ValidationLayers.h
            platforms/vk/VK_PhysicalDevice.cpp
            platforms/vk/VK_PhysicalDevice.h
            platforms/vk/VK_Device.cpp
            platforms/vk/VK_Device.h
            platforms/vk/VK_Swapchain.cpp
            platforms/vk/VK_Swapchain.h
            platforms/vk/VK_Surface.cpp
            platforms/vk/VK_Surface.h
            platforms/vk/VK_GraphicPipeline.cpp
            platforms/vk/VK_GraphicPipeline.h
            platforms/vk/VK_Shader.cpp
            platforms/vk/VK_Shader.h
            platforms/vk/VK_ResourceFactory.cpp
            platforms/vk/VK_ResourceFactory.h
            platforms/vk/VK_Texture.cpp
            platforms/vk/VK_Texture.h
            platforms/vk/VK_Mesh.cpp
            platforms/vk/VK_Mesh.h
            platforms/vk/VK_RenderPass.cpp
            platforms/vk/VK_RenderPass.h
            platforms/vk/ShaderCompiler.cpp
            platforms/vk/ShaderCompiler.h
            platforms/vk/VK_Semaphore.cpp platforms/vk/VK_Semaphore.h platforms/vk/VK_Fence.cpp platforms/vk/VK_Fence.h)

    add_compile_definitions(VULKAN_RENDERER)
else ()
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/")
    add_compile_definitions(OPENGL_RENDERER)
    set(GL_FILES
            platforms/gl/GL_Renderer.cpp
            platforms/gl/GL_Texture.cpp
            platforms/gl/GL_GraphicPipeline.cpp
            platforms/gl/GL_Mesh.cpp
            platforms/gl/GL_Framebuffer.cpp
            platforms/gl/GL_Framebuffer.h
            platforms/gl/GL_Debug.cpp
            platforms/gl/GL_Debug.h
            platforms/gl/GL_MultisamplingFramebuffer.cpp
            platforms/gl/GL_MultisamplingFramebuffer.h
            platforms/gl/GL_Shader.cpp
            platforms/gl/GL_Shader.h
            platforms/gl/GL_Window.cpp
            platforms/gl/GL_Window.h
            platforms/gl/GL_ResourceFactory.cpp
            platforms/gl/GL_ResourceFactory.h)
endif ()


include_directories(.)
set(CORE_FILES
        HBE.h
        Application.cpp
        Application.h
        Configs.cpp
        Core.h
        Configs.h

        core/resource/Texture.h
        core/resource/Texture.cpp
        core/resource/Material.cpp
        core/resource/Material.h
        core/resource/Model.cpp
        core/resource/GraphicPipeline.cpp
        core/resource/GraphicPipeline.h
        core/resource/Shader.cpp

        core/graphics/Window.h
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
        core/entity/component/Transform.cpp
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
        core/utility/ModelImporter.h
        core/threading/JobManager.cpp
        core/threading/JobManager.h
        core/utility/Random.cpp
        core/utility/Random.h

        core/serialization/Serializer.h
        core/serialization/Deserializer.h
        core/serialization/ComponentRegistry.cpp
        core/serialization/ComponentRegistry.h
        core/graphics/Window.cpp
        core/serialization/ComponentRegistry.h
        core/resource/IResourceFactory.h
        core/resource/IMesh.h
        core/resource/Mesh.cpp
        core/resource/Resource.h
        core/resource/ITexture.h
        core/resource/IGraphicPipeline.h
        core/resource/IShader.h
        core/resource/ShaderDB.cpp
        core/resource/ShaderDB.h
        core/graphics/DrawFlags.h
        core/resource/IComputePipeline.h platforms/vk/VK_CommandPool.cpp platforms/vk/VK_CommandPool.h)

add_library(${PROJECT_NAME} SHARED
        ${CORE_FILES}
        ${IMGUI_FILES}
        ${GL_FILES}
        ${VK_FILES}
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/contrib/zlib/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/include/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/include"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/")


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/")
set(GLSLANG_LIBS
        OSDependent
        OGLCompiler
        SPIRV
        glslang
       )

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message("Using Clang not supported")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (WIN32)
        message("Using mingw compiler")
        target_link_libraries(${PROJECT_NAME}
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/mingw/libassimp.a"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/mingw/libzlibstatic.a"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/mingw/libIrrXML.a"

                )
    else ()
        message("Using GCC Compiler")
        target_link_libraries(${PROJECT_NAME}
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/gcc/libassimp.a"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/gcc/libzlibstatic.a"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/gcc/libIrrXML.a")
    endif ()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    error(FATAL_ERROR "Using Intel compiler is not supported")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    message("Using visual studio compiler")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        MESSAGE("debug mode")
        target_link_libraries(${PROJECT_NAME}
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/debug/assimp-vc142-mt.lib"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/debug/zlibstatic.lib"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/debug/IrrXML.lib")
    else ()
        MESSAGE("release mode")
        target_link_libraries(${PROJECT_NAME}
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/release/assimp-vc142-mt.lib"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/release/zlibstatic.lib"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/release/IrrXML.lib")
    endif ()
endif ()
if (VULKAN_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARY})
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE glad)
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${GLSLANG_LIBS})

target_compile_definitions(${PROJECT_NAME} PRIVATE HB_BUILD)
