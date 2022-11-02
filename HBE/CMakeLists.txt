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

#------------------------------------VULKAN------------------------------------
find_package(Vulkan REQUIRED)
if (VULKAN_FOUND)
    add_compile_definitions(VULKAN_RENDERER)
    message("Found vulkan at ${Vulkan_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "Vulkan not found")
endif ()


#------------------------------------GLM-----------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/)
#------------------------------------GLFW------------------------------------
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include")
#-----------------------------------glslang------------------------------------------------------
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/)
#-----------------------------------spirv-cross------------------------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spirv-cross/)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spirv-cross/")
include_directories(${Vulkan_INCLUDE_DIRS})
#-----------------------------------NvidiaRayTracingDenoiser-----------------------------------------------
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/RayTracingDenoiser/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/RayTracingDenoiser/")

#------------------------------TINYGLTF--------------------------------
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_NO_INCLUDE_STB_IMAGE ON CACHE INTERNAL "" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf)

#------------------------------------FREE_TYPE------------------------------------
set(SKIP_INSTALL_ALL TRUE)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_BROTLI ON)
set(FREETYPE_LIBRARY "freetype")
set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype/include")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype")

#------------------------------------MSDFGEN_ATLAS_GEN-------------------
set(MSDFGEN_USE_CPP11 OFF)
set(MSDFGEN_BUILD_STANDALONE OFF)
set(MSDFGEN_INSTALL OFF)
set(MSDFGEN_CORE_ONLY OFF)
set(MSDF_ATLAS_GEN_BUILD_STANDALONE OFF)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/msdf-atlas-gen/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/msdf-atlas-gen/include)

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
        core/resource/GraphicPipelineInstance.h
        core/resource/ComputeInstance.h
        core/resource/IPipelineInstance.h
        core/resource/RenderTarget.h
        core/resource/raytracing/RaytracingPipeline.h
        core/resource/raytracing/RaytracingPipelineInstance.h
        core/resource/TexelBuffer.h

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
        core/resource/StorageBuffer.h
        )
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
        platforms/vk/VK_GraphicPipelineInstance.cpp
        platforms/vk/VK_GraphicPipelineInstance.h
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
        platforms/vk/raytracing/VK_RaytracingPipelineInstance.h
        platforms/vk/VK_StorageBuffer.cpp
        platforms/vk/VK_StorageBuffer.h
        platforms/vk/VK_DescriptorAllocator.cpp
        platforms/vk/VK_DescriptorAllocator.h
        platforms/vk/VK_TexelBuffer.cpp
        platforms/vk/VK_TexelBuffer.h)


add_library(${PROJECT_NAME} SHARED
        ${CORE_FILES}
        ${VK_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE HB_BUILD)

target_include_directories(${PROJECT_NAME}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/RayTracingDenoiser/")


set(GLSLANG_LIBS
        OSDependent
        OGLCompiler
        SPIRV
        glslang
        )
set(SPIRV_CROSS_LIBS
        spirv-cross-glsl
        spirv-cross-cpp
        spirv-cross-c-shared
        spirv-cross-reflect
        spirv-cross-msl
        spirv-cross-util
        spirv-cross-core)

target_link_libraries(${PROJECT_NAME}
         ${Vulkan_LIBRARY}
         glfw
         NRD
        ${SPIRV_CROSS_LIBS}
        ${GLSLANG_LIBS}
        ${FREETYPE_LIBRARY}
        msdfgen
        msdf-atlas-gen
        )

