cmake_minimum_required(VERSION 3.16)
project(imgui)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(backends)
include_directories(misc/cpp)
include_directories(misc/freetype)
include_directories(misc/single_file)

add_library(imgui
        backends/imgui_impl_allegro5.cpp
        backends/imgui_impl_allegro5.h
        backends/imgui_impl_dx10.cpp
        backends/imgui_impl_dx10.h
        backends/imgui_impl_dx11.cpp
        backends/imgui_impl_dx11.h
        backends/imgui_impl_dx12.cpp
        backends/imgui_impl_dx12.h
        backends/imgui_impl_dx9.cpp
        backends/imgui_impl_dx9.h
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_glfw.h
        backends/imgui_impl_glut.cpp
        backends/imgui_impl_glut.h
        backends/imgui_impl_marmalade.cpp
        backends/imgui_impl_marmalade.h
        backends/imgui_impl_metal.h
        backends/imgui_impl_metal.mm
        backends/imgui_impl_opengl2.cpp
        backends/imgui_impl_opengl2.h
        backends/imgui_impl_opengl3.cpp
        backends/imgui_impl_opengl3.h
        backends/imgui_impl_osx.h
        backends/imgui_impl_osx.mm
        backends/imgui_impl_sdl.cpp
        backends/imgui_impl_sdl.h
        backends/imgui_impl_vulkan.cpp
        backends/imgui_impl_vulkan.h
        backends/imgui_impl_win32.cpp
        backends/imgui_impl_win32.h
        misc/cpp/imgui_stdlib.cpp
        misc/cpp/imgui_stdlib.h
        misc/fonts/binary_to_compressed_c.cpp
        misc/freetype/imgui_freetype.cpp
        misc/freetype/imgui_freetype.h
        misc/single_file/imgui_single_file.h
        imconfig.h
        imgui.cpp
        imgui.h
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_internal.h
        imgui_widgets.cpp
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h)
