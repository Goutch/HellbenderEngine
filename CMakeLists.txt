cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(HellbenderEditor CXX)

include_directories("HBE/")
add_subdirectory("HBE/")

include_directories(dependencies/imgui/backends)
include_directories(dependencies/imgui/)
set(IMGUI
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

add_executable (${PROJECT_NAME}
		${IMGUI}
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
		)

target_link_libraries(${PROJECT_NAME} HBE)

