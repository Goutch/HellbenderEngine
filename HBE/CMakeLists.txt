cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(HBE CXX)
add_compile_definitions(HB_BUILD)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(Detected Compiler: MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --std=c++17 -O3 -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -fPIC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(Detected Compiler: MSVC)
    set(MSVC_COMPILE_FLAGS "/MP /Ot")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

endif ()

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
        core/resource/Font.cpp
        core/resource/Font.h

        core/graphics/Window.h
        core/graphics/Renderer.h
        core/graphics/Graphics.cpp
        core/graphics/Graphics.h
        core/graphics/Renderer.cpp
        core/graphics/Window.cpp

        core/input/Input.h
        core/input/Input.cpp

        core/audio/Audio.cpp
        core/audio/Audio.h

        core/scene/ecs/Registry.h
        core/scene/Entity.h
        core/scene/Scene.cpp
        core/scene/System.cpp
        core/scene/System.h
        core/scene/System.cpp
        core/scene/systems/CameraSystem.h
        core/scene/systems/MeshRendererSystem.h
        core/scene/systems/CameraControllerSystem.h
        core/scene/systems/ModelRendererSystem.h
        core/resource/AudioClip.cpp
        core/resource/AudioClip.h
        core/resource/AudioClipInstance.cpp
        core/resource/AudioClipInstance.h

        core/scene/components/Transform.h
        core/scene/components/Camera2D.h
        core/scene/components/Transform2D.h
        core/scene/components/PixelCamera.h
        core/scene/components/Camera.h
        core/scene/components/MeshRenderer.h
        core/scene/components/ModelRenderer.h
        core/scene/components/CameraController.h
        core/scene/components/Transform2D.cpp
        core/scene/components/PixelCamera.cpp
        core/scene/components/MeshRenderer.cpp
        core/scene/components/ModelRenderer.cpp
        core/scene/components/Camera2D.cpp
        core/scene/components/Camera.cpp
        core/scene/components/Transform.cpp
        core/scene/components/CameraController.cpp
        core/scene/ecs/Registry.cpp
        core/scene/ecs/ComponentPool.cpp
        core/scene/ecs/ComponentPool.h
        core/scene/systems/CameraControllerSystem.cpp
        core/scene/systems/CameraSystem.cpp
        core/scene/systems/MeshRendererSystem.cpp
        core/scene/systems/ModelRendererSystem.cpp
        core/scene/ecs/ComponentTypeRegistry.h

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
        core/graphics/RenderGraph.cpp
        core/graphics/RenderGraph.h
        core/scene/ecs/Group.cpp core/scene/ecs/Group.h core/scene/ecs/RegistryPage.cpp core/scene/ecs/RegistryPage.h)
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
set(BUILD_SHARED_LIBS FALSE)
set(GLFW_BUILD_EXAMPLES FALSE)
set(GLFW_BUILD_TESTS FALSE)
set(GLFW_BUILD_DOCS FALSE)
set(GLFW_INSTALL FALSE)
set(GLFW_VULKAN_STATIC FALSE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include")
#set_property(TARGET glfw PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#-----------------------------------glslang------------------------------------------------------
set(BUILD_SHARED_LIBS FALSE)
set(BUILD_EXTERNAL TRUE)
set(ENABLE_GLSLANG_BINARIES FALSE)
set(ENABLE_CTEST FALSE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/)
#set_property(TARGET glslang PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#-----------------------------------spirv-cross------------------------------------------------------
#set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS "Instead of throwing exceptions assert" FALSE)
#set(SPIRV_CROSS_SHARED "Build the C API as a single shared library." TRUE)
#set(SPIRV_CROSS_STATIC "Build the C and C++ API as static libraries." TRUE)
#set(SPIRV_CROSS_CLI "Build the CLI binary. Requires SPIRV_CROSS_STATIC." FALSE)
#set(SPIRV_CROSS_ENABLE_TESTS "Enable SPIRV-Cross tests." FALSE)
#
#set(SPIRV_CROSS_ENABLE_GLSL "Enable GLSL support." TRUE)
#set(SPIRV_CROSS_ENABLE_HLSL "Enable HLSL target support." TRUE)
#set(SPIRV_CROSS_ENABLE_MSL "Enable MSL target support." TRUE)
#set(SPIRV_CROSS_ENABLE_CPP "Enable C++ target support." TRUE)
#set(SPIRV_CROSS_ENABLE_REFLECT "Enable JSON reflection target support." TRUE)
#set(SPIRV_CROSS_ENABLE_C_API "Enable C API wrapper support in static library." TRUE)
#set(SPIRV_CROSS_ENABLE_UTIL "Enable util module support." TRUE)
#
#set(SPIRV_CROSS_FORCE_STL_TYPES "Force use of STL types instead of STL replacements in certain places. Might reduce performance." FALSE)
#
#set(SPIRV_CROSS_SKIP_INSTALL "Skips installation targets." TRUE)
#
#set(SPIRV_CROSS_FORCE_PIC "Force position-independent code for all targets." FALSE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spirv-cross/)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spirv-cross/")
include_directories(${Vulkan_INCLUDE_DIRS})
#set_property(TARGET spirv-cross-glsl PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET spirv-cross-glsl spirv-cross-cpp PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET spirv-cross-c-shared PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET spirv-cross-reflect PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET spirv-cross-msl PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET spirv-cross-util PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET spirv-cross-core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET SPIRV PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET MachineIndependent PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET GenericCodeGen PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_property(TARGET OGLCompiler PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#-----------------------------------NvidiaRayTracingDenoiser-----------------------------------------------
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/RayTracingDenoiser/")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/RayTracingDenoiser/")

#------------------------------TINYGLTF--------------------------------
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_NO_INCLUDE_STB_IMAGE ON CACHE INTERNAL "" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf)
set_property(TARGET tinygltf PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#------------------------------------FREE_TYPE------------------------------------
set(SKIP_INSTALL_ALL TRUE)
set(FT_DISABLE_ZLIB TRUE)
set(FT_DISABLE_BZIP2 TRUE)
set(FT_DISABLE_PNG TRUE)
set(FT_DISABLE_HARFBUZZ TRUE)
set(FT_DISABLE_BROTLI TRUE)

set(FREETYPE_LIBRARY freetype)
set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype/include")
set(FREETYPE_INCLUDE_DIR_ft2build "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype")
set(FREETYPE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype")


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype")
add_library(Freetype::Freetype ALIAS freetype)
#------------------------------------MSDFGEN-----------------------------------
set(MSDFGEN_USE_CPP11 TRUE)
set(MSDFGEN_BUILD_STANDALONE FALSE)
set(MSDFGEN_INSTALL FALSE)
set(MSDFGEN_CORE_ONLY FALSE)
set(MSDFGEN_USE_SKIA FALSE)

#------------------------------------MSDFGEN_ATLAS_GEN-------------------
set(MSDF_ATLAS_GEN_BUILD_STANDALONE FALSE)
set(MSDF_ATLAS_GEN_MSDFGEN_EXTERNAL FALSE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/msdf-atlas-gen/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/msdf-atlas-gen/include)
set_property(TARGET msdf-atlas-gen PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_property(TARGET msdfgen-core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_property(TARGET msdfgen-ext PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

#------------------------------------OPENAL SOFT -----------------------------------

set(LIBTYPE STATIC)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/openal-soft/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/openal-soft/include)

#------------------------------------LIBSNDFILE------------------------------------
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libsndfile/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libsndfile/include)



include_directories(.)


add_library(${PROJECT_NAME} SHARED
        ${CORE_FILES}
        ${VK_FILES})


if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE HB_BUILD)

target_include_directories(${PROJECT_NAME}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb/"
        #PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/RayTracingDenoiser/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf/"
        PUBLIC "${FREETYPE_INCLUDE_DIRS}")


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
        #NRD
        ${SPIRV_CROSS_LIBS}
        ${GLSLANG_LIBS}
        ${FREETYPE_LIBRARY}
        msdfgen-all
        msdf-atlas-gen
        OpenAL
        SndFile::sndfile
        )
