cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(HBE CXX)
add_compile_definitions(HB_BUILD)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --std=c++17 -O3 -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -fPIC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(MSVC_COMPILE_FLAGS "/MP /Ot")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif ()
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Vulkan REQUIRED)
if (VULKAN_FOUND)
    message("Found vulkan at ${Vulkan_INCLUDE_DIRS}!")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spirv-cross/)
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
            platforms/vk/VK_Image.cpp
            platforms/vk/VK_Image.h
            platforms/vk/VK_Mesh.h
            platforms/vk/VK_Mesh.cpp
            platforms/vk/VK_RenderPass.cpp
            platforms/vk/VK_RenderPass.h
            platforms/vk/ShaderCompiler.cpp
            platforms/vk/ShaderCompiler.h
            platforms/vk/VK_Semaphore.cpp
            platforms/vk/VK_Semaphore.h
            platforms/vk/VK_Fence.cpp
            platforms/vk/VK_Fence.h
            platforms/vk/VK_CommandPool.cpp
            platforms/vk/VK_CommandPool.h
            platforms/vk/VK_Buffer.cpp
            platforms/vk/VK_Buffer.h
            platforms/vk/VK_Memory.cpp
            platforms/vk/VK_Memory.h
            platforms/vk/VK_Allocator.cpp
            platforms/vk/VK_Allocator.h
            platforms/vk/VK_CONSTANTS.h
            platforms/vk/VK_Queue.cpp
            platforms/vk/VK_Queue.h
            platforms/vk/VK_ComputePipeline.cpp
            platforms/vk/VK_ComputePipeline.h
            platforms/vk/VK_PipelineLayout.cpp
            platforms/vk/VK_PipelineLayout.h
            platforms/vk/VK_PipelineDescriptors.cpp
            platforms/vk/VK_PipelineDescriptors.h
            platforms/vk/VK_Material.cpp
            platforms/vk/VK_Material.h
            platforms/vk/VK_ComputeInstance.cpp
            platforms/vk/VK_ComputeInstance.h
            platforms/vk/raytracing/VK_TopLevelAccelerationStructure.cpp
            platforms/vk/raytracing/VK_TopLevelAccelerationStructure.h
            platforms/vk/raytracing/VK_MeshBottomLevelAccelerationStructure.cpp
            platforms/vk/raytracing/VK_MeshBottomLevelAccelerationStructure.h
            platforms/vk/raytracing/VK_AABBBottomLevelAccelerationStructure.cpp
            platforms/vk/raytracing/VK_MeshBottomLevelAccelerationStructure.cpp
            platforms/vk/raytracing/VK_RaytracingPipeline.cpp
            platforms/vk/raytracing/VK_RaytracingPipeline.h
            platforms/vk/raytracing/VK_RaytracingPipelineInstance.cpp
            platforms/vk/raytracing/VK_RaytracingPipelineInstance.h)

    add_compile_definitions(VULKAN_RENDERER)


    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spirv-cross/")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/")
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


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/")

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_NO_INCLUDE_STB_IMAGE ON CACHE INTERNAL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf)
include_directories(.)
set(CORE_FILES
        HBE.h
        Application.cpp
        Application.h
        Configs.cpp
        Core.h
        Configs.h
        HBETypes.h

        core/resource/Texture.h
        core/resource/Model.cpp
        core/resource/Model.h
        core/resource/Mesh.cpp
        core/resource/Mesh.h
        core/resource/Shader.cpp
        core/resource/Shader.h
        core/resource/ResourceFactory.h
        core/resource/Resource.h
        core/resource/GraphicPipeline.h
        core/resource/ComputePipeline.h
        core/resource/GLSL_TYPE.h
        core/resource/Resources.h
        core/resource/Resources.cpp
        core/resource/Texture.cpp
        core/resource/Material.h
        core/resource/ComputeInstance.h
        core/resource/IPipelineInstance.h
        core/resource/RenderTarget.h
        core/resource/raytracing/RaytracingPipeline.h
        core/resource/raytracing/RaytracingPipelineInstance.h

        core/graphics/Window.h
        core/graphics/Renderer.h
        core/graphics/Graphics.cpp
        core/graphics/Graphics.h
        core/graphics/Renderer.cpp
        core/graphics/Window.cpp

        core/input/Input.h
        core/input/Input.cpp

        core/scene/ecs/Registry.h
        core/scene/Scene.cpp
        core/scene/System.h
        core/scene/Components.h
        core/scene/Components.cpp
        core/scene/systems/CameraSystem.cpp
        core/scene/systems/CameraSystem.h
        core/scene/systems/MeshRendererSystem.cpp
        core/scene/systems/MeshRendererSystem.h
        core/scene/systems/CameraControllerSystem.cpp
        core/scene/systems/CameraControllerSystem.h
        core/scene/systems/RotatorSystem.cpp
        core/scene/systems/RotatorSystem.h
        core/scene/systems/ModelRendererSystem.cpp
        core/scene/systems/ModelRendererSystem.h

        core/utility/Log.cpp
        core/utility/Geometry.cpp
        core/utility/Geometry.h
        core/utility/Clock.cpp
        core/utility/Random.cpp
        core/utility/Random.h
        core/utility/RawVector.h
        core/utility/Profiler.cpp
        core/utility/Profiler.h

        core/threading/JobManager.cpp
        core/threading/JobManager.h
        )

add_library(${PROJECT_NAME} SHARED
        ${CORE_FILES}
        ${IMGUI_FILES}
        ${GL_FILES}
        ${VK_FILES}
        )
target_compile_definitions(${PROJECT_NAME} PRIVATE HB_BUILD)

target_include_directories(${PROJECT_NAME}
        PUBLIC "${${CMAKE_CURRENT_SOURCE_DIR}}"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/contrib/zlib/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/entt/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/include/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/include"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spirv-cross/")

if (VULKAN_FOUND)
    set(GLSLANG_LIBS
            OSDependent
            OGLCompiler
            SPIRV
            glslang
            )
    target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARY})
    target_link_libraries(${PROJECT_NAME}
            PUBLIC spirv-cross-glsl
            PUBLIC spirv-cross-cpp
            PUBLIC spirv-cross-c-shared
            PUBLIC spirv-cross-reflect
            PUBLIC spirv-cross-msl
            PUBLIC spirv-cross-util
            PUBLIC spirv-cross-core)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC glad)
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${GLSLANG_LIBS})

