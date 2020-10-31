cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(HellbenderEngine)

include_directories("dependencies/glm")
add_subdirectory("dependencies/glm")

include_directories("dependencies/glfw/include")
add_subdirectory("dependencies/glfw")

include_directories("dependencies/glad/include")
add_subdirectory("dependencies/glad")

include_directories("dependencies/stb")
add_subdirectory("dependencies/stb")

include_directories("dependencies/HellbenderUtility")
add_subdirectory("dependencies/HellbenderUtility")

include_directories("HBE")

set(CORE_FILES
		HBE/Configs.cpp
		HBE/core/Core.h
        HBE/core/HBE.cpp
        HBE/core/HBE.h

		HBE/CompilationConfigs.h
        HBE/Configs.h

		HBE/core/resource/ShaderProgram.h
		HBE/core/resource/Texture.h
		HBE/core/resource/Framebuffer.h
		HBE/core/resource/Resource.cpp
		HBE/core/resource/Texture.cpp
		HBE/core/resource/Material.cpp
		HBE/core/resource/Material.h
		HBE/core/resource/MaterialProperty.h

        HBE/core/graphics/IRenderer.h
		HBE/core/graphics/Graphics.cpp
		HBE/core/graphics/Graphics.h
		HBE/core/graphics/Layer.cpp
		HBE/core/graphics/Layer.h

		HBE/core/input/Input.h
		HBE/core/input/Input.cpp

		HBE/core/entity/Scene.cpp
		HBE/core/entity/Scene.h
		HBE/core/entity/Entity.cpp
		HBE/core/entity/Transform.cpp
		HBE/core/entity/component/Camera.cpp
		HBE/core/entity/component/Component.h
		HBE/core/entity/component/CameraController.cpp
		HBE/core/entity/component/MeshRenderer.cpp
		HBE/core/entity/component/MeshRenderer.h
		HBE/core/entity/component/CameraController.h
		HBE/core/entity/component/Component.cpp
		HBE/core/entity/component/InstancesRenderer.cpp
		HBE/core/entity/component/InstancesRenderer.h

		HBE/core/utility/Log.cpp
		HBE/core/utility/Geometry.cpp
		HBE/core/utility/Geometry.h
		)

set(GL_FILES
		HBE/graphics/gl/GL_Core.h
		HBE/graphics/gl/GL_Renderer.cpp
 		HBE/graphics/gl/GL_Texture.cpp
		HBE/graphics/gl/GL_ShaderProgram.cpp
		HBE/graphics/gl/GL_Mesh.cpp
		HBE/graphics/gl/GL_Framebuffer.cpp
		HBE/graphics/gl/GL_Framebuffer.h
		HBE/graphics/gl/GL_Debug.cpp
		HBE/graphics/gl/GL_Debug.h
		HBE/graphics/gl/GL_MultisamplingFramebuffer.cpp
		HBE/graphics/gl/GL_MultisamplingFramebuffer.h )

add_library (${PROJECT_NAME}
		${CORE_FILES}
		${GL_FILES}
		)

target_link_libraries(${PROJECT_NAME} HBU glm glfw glad stb)
