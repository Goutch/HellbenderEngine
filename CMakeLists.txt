cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(Framework)

include_directories("dependencies/glm")
add_subdirectory("dependencies/glm")

include_directories("dependencies/glfw/include")
add_subdirectory("dependencies/glfw")

include_directories("dependencies/glad/include")
add_subdirectory("dependencies/glad")

include_directories("dependencies/stb")
add_subdirectory("dependencies/stb")

include_directories("src")

set(CORE_FILES
		src/core/Core.h
		src/core/PrototypeEngine.cpp
		src/core/PrototypeEngine.h

		src/CompilationConfigs.h

		src/core/resource/ShaderProgram.h
		src/core/resource/Texture.h
		src/core/resource/Framebuffer.h
		src/core/resource/Resource.cpp
		src/core/resource/Texture.cpp

        src/core/graphics/IRenderer.h
		src/core/graphics/Graphics.cpp
		src/core/graphics/Graphics.h
		src/core/graphics/Layer.cpp
		src/core/graphics/Layer.h

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
		src/core/entity/component/Component.cpp

		src/core/utility/Log.cpp
		src/core/utility/Geometry.cpp
		src/core/utility/Geometry.h
		src/core/utility/Timer.cpp
		src/core/utility/Event.h
        src/core/entity/component/InstancesRenderer.cpp src/core/entity/component/InstancesRenderer.h src/core/resource/Material.cpp src/core/resource/Material.h)

set(GL_FILES
		src/graphics/gl/GL_Core.h
		src/graphics/gl/GL_Renderer.cpp
 		src/graphics/gl/GL_Texture.cpp
		src/graphics/gl/GL_ShaderProgram.cpp
		src/graphics/gl/GL_Mesh.cpp
		src/graphics/gl/GL_Framebuffer.cpp
		src/graphics/gl/GL_Framebuffer.h
		src/graphics/gl/GL_Debug.cpp
		src/graphics/gl/GL_Debug.h
		src/graphics/gl/GL_MultisamplingFramebuffer.cpp
		src/graphics/gl/GL_MultisamplingFramebuffer.h src/core/resource/GLSL_TYPE.h src/RuntimeConfigs.h)

add_library (${PROJECT_NAME}
		${CORE_FILES}
		${GL_FILES}
		)

target_link_libraries(${PROJECT_NAME} glm glfw glad stb)
