cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
project(HBE CXX)

if (WIN32)
	execute_process(COMMAND cmd /C ${CMAKE_CURRENT_SOURCE_DIR}/UpdateSubmodules.bat)
else (UNIX)
	execute_process(COMMAND cmd /C ${CMAKE_CURRENT_SOURCE_DIR}/UpdateSubmodules.sh)
endif ()

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

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/utils-collection/)
	message(FATAL_ERROR "HBE repository is incomplete, initialize submodules using:\n  git submodule update --init --recursive")
endif ()


set(CORE_FILES
		HBE.h
		Application.cpp
		Application.h
		Configs.cpp
		Core.h
		Configs.h
		HBETypes.h

		core/resource/Model.cpp
		core/resource/Model.h
		core/resource/Mesh.cpp
		core/resource/Mesh.h
		core/resource/Shader.cpp
		core/resource/Shader.h
		core/resource/ResourceFactory.h
		core/resource/Resource.h
		core/resource/RaterizationPipeline.h
		core/resource/ComputePipeline.h
		core/resource/Resources.h
		core/resource/Resources.cpp
		core/resource/Image.cpp
		core/resource/RasterizationPipelineInstance.h
		core/resource/ComputeInstance.h
		core/resource/IPipelineInstance.h
		core/resource/RasterizationTarget.h
		core/resource/raytracing/RaytracingPipeline.h
		core/resource/raytracing/RaytracingPipelineInstance.h
		core/resource/TexelBuffer.h
		core/resource/Font.cpp
		core/resource/Font.h
		core/resource/StorageBuffer.h
		core/resource/AudioClip.cpp
		core/resource/AudioClip.h
		core/resource/AudioClipInstance.cpp
		core/resource/AudioClipInstance.h

		core/graphics/Window.h
		core/graphics/Renderer.h
		core/graphics/Graphics.cpp
		core/graphics/Graphics.h
		core/graphics/Renderer.cpp
		core/graphics/Window.cpp
		core/graphics/RenderGraph.cpp
		core/graphics/RenderGraph.h
		core/graphics/GraphicLimits.h

		core/input/Input.h
		core/input/Input.cpp

		core/audio/Audio.cpp
		core/audio/Audio.h

		core/scene/ecs/Registry.h
		core/scene/ecs/Registry.cpp
		core/scene/ecs/ComponentPool.cpp
		core/scene/ecs/ComponentPool.h
		core/scene/ecs/ComponentTypeRegistry.h
		core/scene/ecs/Group.cpp
		core/scene/ecs/Group.h
		core/scene/ecs/RegistryPage.cpp
		core/scene/ecs/RegistryPage.h
		core/scene/ecs/Component.h
		core/scene/Entity.h
		core/scene/Scene.cpp
		core/scene/System.cpp
		core/scene/System.h
		core/scene/System.cpp
		core/scene/systems/CameraSystem.h
		core/scene/systems/MeshRendererSystem.h
		core/scene/systems/CameraControllerSystem.h
		core/scene/systems/ModelRendererSystem.h
		core/scene/systems/CameraControllerSystem.cpp
		core/scene/systems/CameraSystem.cpp
		core/scene/systems/ui/UIPanelSystem.cpp
		core/scene/systems/ui/ButtonSystem.cpp
		core/scene/systems/ui/TextBoxSystem.cpp
		core/scene/systems/MeshRendererSystem.cpp
		core/scene/systems/ModelRendererSystem.cpp
		core/scene/systems/HierarchySystem.cpp
		core/scene/systems/ui/LabelSystem.cpp
		core/scene/systems/TransformSystem.cpp
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
		core/scene/components/EntityState.cpp
		core/scene/components/EntityState.h
		core/scene/components/CameraController.cpp
		core/scene/components/HierarchyNode.cpp
		core/scene/ecs/Component.h

		core/utility/Log.cpp
		core/utility/Geometry.cpp
		core/utility/Geometry.h
		core/utility/Profiler.cpp
		core/utility/Profiler.h
		core/utility/GLTFLoader.cpp
		core/utility/GLTFLoader.h
		core/utility/DefaultModelParser.cpp
		core/utility/DefaultModelParser.h


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
		platforms/vk/VK_RasterizationPipeline.cpp
		platforms/vk/VK_RasterizationPipeline.h
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
		platforms/vk/VK_Queue.cpp
		platforms/vk/VK_Queue.h
		platforms/vk/VK_ComputePipeline.cpp
		platforms/vk/VK_ComputePipeline.h
		platforms/vk/VK_PipelineLayout.cpp
		platforms/vk/VK_PipelineLayout.h
		platforms/vk/VK_PipelineDescriptors.cpp
		platforms/vk/VK_PipelineDescriptors.h
		platforms/vk/VK_RasterizationPipelineInstance.cpp
		platforms/vk/VK_RasterizationPipelineInstance.h
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
		platforms/vk/VK_TexelBuffer.cpp
		platforms/vk/VK_TexelBuffer.h
		platforms/vk/VK_Utils.h
		platforms/vk/VK_DescriptorSetLayout.cpp
		platforms/vk/VK_DescriptorSetLayout.h
		platforms/vk/VK_RenderTarget.cpp
		platforms/vk/VK_RenderTarget.h
		platforms/vk/VK_DynamicRenderTarget.cpp
		platforms/vk/VK_DynamicRenderTarget.h
		core/graphics/Allocator.h)


include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/utils-collection/")
#------------------------------------VULKAN------------------------------------
find_package(Vulkan REQUIRED)
if (VULKAN_FOUND)
	add_compile_definitions(VULKAN_RENDERER)
	message("Found vulkan at ${Vulkan_INCLUDE_DIRS}")
else ()
	message(FATAL_ERROR "Vulkan not found")
endif ()


message("#------------------------------------GLM-----------------------------------")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/)
include_directories(/dependencies/glm/)

message("#------------------------------------GLFW------------------------------------")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL FALSE)
set(GLFW_BUILD_X11 OFF CACHE BOOL "")
set(GLFW_VULKAN_STATIC FALSE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include")
#set_property(TARGET glfw PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

message("#-----------------------------------glslang------------------------------------------------------")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(BUILD_EXTERNAL TRUE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "")
set(ENABLE_CTEST OFF CACHE BOOL "")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang/)
#set_property(TARGET glslang PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

message("#-----------------------------------spirv-cross------------------------------------------------------")
set(SPIRV_CROSS_SHARED FALSE)
set(SPIRV_CROSS_STATIC TRUE)
set(SPIRV_CROSS_ENABLE_C_API TRUE)
set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS OFF CACHE BOOL "Instead of throwing exceptions assert")
set(SPIRV_CROSS_CLI OFF CACHE BOOL "Build the CLI binary. Requires SPIRV_CROSS_STATIC.")
set(SPIRV_CROSS_ENABLE_TESTS  OFF CACHE BOOL "Enable SPIRV-Cross tests.")
#set(SPIRV_CROSS_ENABLE_GLSL "Enable GLSL support." TRUE)
#set(SPIRV_CROSS_ENABLE_HLSL "Enable HLSL target support." TRUE)
#set(SPIRV_CROSS_ENABLE_MSL "Enable MSL target support." TRUE)
#set(SPIRV_CROSS_ENABLE_CPP "Enable C++ target support." TRUE)
#set(SPIRV_CROSS_ENABLE_REFLECT "Enable JSON reflection target support." TRUE)
#set(SPIRV_CROSS_ENABLE_UTIL "Enable util module support." TRUE)
#set(SPIRV_CROSS_FORCE_STL_TYPES "Force use of STL types instead of STL replacements in certain places. Might reduce performance." FALSE)
#set(SPIRV_CROSS_SKIP_INSTALL "Skips installation targets." TRUE)
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

message("#------------------------------TINYGLTF--------------------------------")
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_NO_INCLUDE_STB_IMAGE ON CACHE INTERNAL "" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf)
set_property(TARGET tinygltf PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
message("#------------------------------------FREE_TYPE------------------------------------")
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

message("#------------------------------------MSDFGEN-----------------------------------")
set(MSDFGEN_USE_CPP11 TRUE)
set(MSDFGEN_BUILD_STANDALONE FALSE)
set(MSDFGEN_INSTALL FALSE)
set(MSDFGEN_CORE_ONLY FALSE)
set(MSDFGEN_USE_SKIA FALSE)

message("#------------------------------------MSDFGEN_ATLAS_GEN-------------------")
set(MSDF_ATLAS_GEN_BUILD_STANDALONE FALSE)
set(MSDF_ATLAS_GEN_MSDFGEN_EXTERNAL FALSE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/msdf-atlas-gen/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/msdf-atlas-gen/include)
set_property(TARGET msdf-atlas-gen PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_property(TARGET msdfgen-core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_property(TARGET msdfgen-ext PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

message("#------------------------------------OPENAL SOFT -----------------------------------")

set(LIBTYPE STATIC)

#set(ALSOFT_DLOPEN OFF CACHE BOOL "Check for the dlopen API for loading optional libs" )
#set(ALSOFT_WERROR OFF CACHE BOOL "Treat compile warnings as errors")
set(ALSOFT_UTILS OFF CACHE BOOL "Build utility programs")
set(ALSOFT_NO_CONFIG_UTIL ON CACHE BOOL "Disable building the alsoft-config utility")
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build example programs")
set(ALSOFT_TESTS OFF CACHE BOOL "Build test programs")
#set(ALSOFT_INSTALL OFF CACHE BOOL "Install main library")
#set(ALSOFT_INSTALL_CONFIG  OFF CACHE BOOL "Install alsoft.conf sample configuration file")
#set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL"Install HRTF data files")
#set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "Install AmbDec preset files")
#set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "Install example programs (alplay, alstream, ...)")
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "Install utility programs (openal-info, alsoft-config, ...)")
#set(ALSOFT_UPDATE_BUILD_VERSION OFF CACHE BOOL "Update git build version info")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/openal-soft/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/openal-soft/include)

message("#------------------------------------LIBSNDFILE------------------------------------")
set (BUILD_PROGRAMS OFF CACHE BOOL "")
set (BUILD_EXAMPLES OFF CACHE BOOL "")
set (ENABLE_CPACK OFF CACHE BOOL "")
set(BUILD_TESTING OFF CACHE BOOL "Set LibSNDFile test to Off")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libsndfile/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libsndfile/include)

message("#------------------------------------HBE------------------------------------")
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
		PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tinygltf/"
		PUBLIC "${FREETYPE_INCLUDE_DIRS}"
		PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/utils-collection")


set(GLSLANG_LIBS
		OSDependent
		OGLCompiler
		SPIRV
		glslang
)
set(GLFW_LIBS
		glfw)

set(SPIRV_CROSS_LIBS
		spirv-cross-glsl
		spirv-cross-cpp
		spirv-cross-c
		spirv-cross-reflect
		spirv-cross-msl
		spirv-cross-util
		spirv-cross-core)

set(MSDFGEN_LIBS
		msdfgen-all
		msdf-atlas-gen)
set(OPENAL_LIBS
		OpenAL)
set(SNDFILE_LIBS
		sndfile)

set(HBE_LIBS
		${Vulkan_LIBRARY}
		${GLFW_LIBS}
		${SPIRV_CROSS_LIBS}
		${GLSLANG_LIBS}
		${FREETYPE_LIBRARY}
		${MSDFGEN_LIBS}
		${OPENAL_LIBS}
		${SNDFILE_LIBS})

target_link_libraries(${PROJECT_NAME}
		${HBE_LIBS})

if (NOT DEFINED HBE_OUTPUT_DIR)
	set(HBE_OUTPUT_DIR ${CMAKE_BINARY_DIR})
endif ()

set_target_properties(${PROJECT_NAME}
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${HBE_OUTPUT_DIR})

