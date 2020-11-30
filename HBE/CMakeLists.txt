cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(HBE CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --std=c++17 -O3 -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -fPIC")
endif ()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lua/")


include_directories(.)
set(CORE_FILES
        HBE.h
        Application.cpp
        Application.h
        Configs.cpp
        Core.h
        Configs.h

        core/resource/ShaderProgram.h
        core/resource/Texture.h
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
        core/utility/ModelImporter.cpp core/utility/ModelImporter.h
        core/utility/JobManager.cpp
        core/utility/JobManager.h
        core/utility/Random.cpp
        core/utility/Random.h

        core/scripting/LuaDeserializer.cpp
        core/scripting/LuaDeserializer.h
        core/scripting/ScriptManager.cpp
        core/scripting/ScriptManager.h

        core/serialization/Serializer.h
        core/serialization/Deserializer.h
        core/serialization/ComponentRegistry.cpp
        core/serialization/ComponentRegistry.h)

set(IMGUI_FILES
        #dependencies/imgui/backends/imgui_impl_allegro5.cpp
        #dependencies/imgui/backends/imgui_impl_allegro5.h
        #dependencies/imgui/backends/imgui_impl_dx10.cpp
        #dependencies/imgui/backends/imgui_impl_dx10.h
        #dependencies/imgui/backends/imgui_impl_dx11.cpp
        #dependencies/imgui/backends/imgui_impl_dx11.h
        #dependencies/imgui/backends/imgui_impl_dx12.cpp
        #dependencies/imgui/backends/imgui_impl_dx12.h
        #dependencies/imgui/backends/imgui_impl_dx9.cpp
        #dependencies/imgui/backends/imgui_impl_dx9.h
        dependencies/imgui/backends/imgui_impl_glfw.cpp
        dependencies/imgui/backends/imgui_impl_glfw.h
        #dependencies/imgui/backends/imgui_impl_glut.cpp
        #dependencies/imgui/backends/imgui_impl_glut.h
        #dependencies/imgui/backends/imgui_impl_marmalade.cpp
        #dependencies/imgui/backends/imgui_impl_marmalade.h
        #dependencies/imgui/backends/imgui_impl_metal.h
        #dependencies/imgui/backends/imgui_impl_metal.mm
        #dependencies/imgui/backends/imgui_impl_opengl2.cpp
        #dependencies/imgui/backends/imgui_impl_opengl2.h
        dependencies/imgui/backends/imgui_impl_opengl3.cpp
        dependencies/imgui/backends/imgui_impl_opengl3.h
        #dependencies/imgui/backends/imgui_impl_osx.h
        #dependencies/imgui/backends/imgui_impl_osx.mm
        #dependencies/imgui/backends/imgui_impl_sdl.cpp
        #dependencies/imgui/backends/imgui_impl_sdl.h
        #dependencies/imgui/backends/imgui_impl_vulkan.cpp
        #dependencies/imgui/backends/imgui_impl_vulkan.h
        #dependencies/imgui/backends/imgui_impl_win32.cpp
        #dependencies/imgui/backends/imgui_impl_win32.h
        #dependencies/imgui/misc/cpp/imgui_stdlib.cpp
        #dependencies/imgui/misc/cpp/imgui_stdlib.h
        #dependencies/imgui/misc/fonts/binary_to_compressed_c.cpp
        #dependencies/imgui/misc/freetype/imgui_freetype.cpp
        #dependencies/imgui/misc/freetype/imgui_freetype.h
        #dependencies/imgui/misc/single_file/imgui_single_file.h
        dependencies/imgui/imconfig.h
        dependencies/imgui/imgui.cpp
        dependencies/imgui/imgui.h
        dependencies/imgui/imgui_demo.cpp
        dependencies/imgui/imgui_draw.cpp
        dependencies/imgui/imgui_internal.h
        dependencies/imgui/imgui_widgets.cpp
        dependencies/imgui/imstb_rectpack.h
        dependencies/imgui/imstb_textedit.h
        dependencies/imgui/imstb_truetype.h)

set(GL_FILES
        platforms/gl/GL_Core.h
        platforms/gl/GL_Renderer.cpp
        platforms/gl/GL_Texture.cpp
        platforms/gl/GL_ShaderProgram.cpp
        platforms/gl/GL_Mesh.cpp
        platforms/gl/GL_Framebuffer.cpp
        platforms/gl/GL_Framebuffer.h
        platforms/gl/GL_Debug.cpp
        platforms/gl/GL_Debug.h
        platforms/gl/GL_MultisamplingFramebuffer.cpp
        platforms/gl/GL_MultisamplingFramebuffer.h)

add_library(${PROJECT_NAME} SHARED
        ${CORE_FILES}
        ${IMGUI_FILES}
        ${GL_FILES}
      )

target_include_directories(${PROJECT_NAME}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/contrib/zlib/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/include/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/include"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lua/src/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message("Using Clang")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    if (WIN32)
        message("Using mingw compiler")
        target_link_libraries(${PROJECT_NAME}
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/mingw/libassimp.a"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/mingw/libzlibstatic.a"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/mingw/libIrrXML.a"
                PUBLIC glfw
                PUBLIC glad
                PUBLIC lua)
    else ()
        message("Using GCC Compiler")
        target_link_libraries(${PROJECT_NAME}
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/gcc/libassimp.a"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/gcc/libzlibstatic.a"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/gcc/libIrrXML.a"
                PUBLIC glfw
                PUBLIC glad
                PUBLIC lua)
    endif ()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    error(FATAL_ERROR "Using Intel compiler is not supported")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    message("Using visual studio compiler")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        MESSAGE("debug mode ")
        target_link_libraries(${PROJECT_NAME}
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/debug/assimp-vc142-mt.lib"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/debug/zlibstatic.lib"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/debug/IrrXML.lib"
                PUBLIC glfw
                PUBLIC glad
                PUBLIC lua)
    else ()
        MESSAGE("release mode")
        target_link_libraries(${PROJECT_NAME}
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/release/assimp-vc142-mt.lib"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/release/zlibstatic.lib"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/vc/release/IrrXML.lib"
                PUBLIC glfw
                PUBLIC glad
                PUBLIC lua)
    endif ()
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE HB_BUILD)
