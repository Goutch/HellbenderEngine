cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(App)

include_directories("dependencies/glm")
add_subdirectory("dependencies/glm")

include_directories("dependencies/glfw/include")
add_subdirectory("dependencies/glfw")

include_directories("dependencies/glad/include")
add_subdirectory("dependencies/glad")

#include_directories("dependencies/assimp/include")
#add_subdirectory("dependencies/assimp/")

include_directories("src")

set(CORE_FILES
		src/Core.h

		src/core/PrototypeEngine.cpp
		src/core/PrototypeEngine.h
		src/Configs.h

		src/core/ressource/ShaderProgram.h
		src/core/ressource/Texture.h
		src/core/ressource/Framebuffer.h

        src/core/graphics/Renderer.h
		src/core/graphics/Graphics.cpp
		src/core/graphics/Graphics.h

		src/core/input/Input.h
		src/core/input/Input.cpp

		src/core/entity/Scene.cpp
		src/core/entity/Scene.h
		src/core/entity/Entity.cpp
		src/core/entity/Transform.cpp
		src/core/entity/component/Camera.cpp
		src/core/entity/component/Component.h
		src/core/entity/component/CameraController.cpp
		src/core/entity/component/MeshRenderer.cpp
		src/core/entity/component/MeshRenderer.h
		src/core/entity/component/CameraController.h

		src/core/utility/Log.cpp
		src/core/utility/Geometry.cpp
		src/core/utility/Geometry.h
		src/core/utility/Timer.cpp
		)

set(GL_FILES
		src/graphics/gl/GL_Renderer.cpp
 		src/graphics/gl/GL_Texture.cpp
		src/graphics/gl/GL_ShaderProgram.cpp
		src/graphics/gl/GL_Mesh.cpp
		src/graphics/gl/GL_Framebuffer.cpp
		src/graphics/gl/GL_Framebuffer.h
		src/graphics/gl/GL_Debug.cpp
		src/graphics/gl/GL_Debug.h src/core/graphics/Layer.cpp src/core/graphics/Layer.h src/graphics/gl/GL_MultisamplingFramebuffer.cpp src/graphics/gl/GL_MultisamplingFramebuffer.h)

add_executable (${PROJECT_NAME}
		src/main.cpp
		${CORE_FILES}
		${GL_FILES}
		)

target_link_libraries(${PROJECT_NAME} glfw glad)
