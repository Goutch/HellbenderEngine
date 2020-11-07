cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(HBE CXX)
SET(CMAKE_C_LINK_EXECUTABLE ${CMAKE_CXX_LINK_EXECUTABLE})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/include")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/include")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lua/src")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lua/")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(CORE_FILES
		HBE.cpp
		HBE.h
		Configs.cpp

		CompilationConfigs.h
        Configs.h

		core/resource/ShaderProgram.h
		core/resource/Texture.h
		core/resource/Resource.cpp
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
		core/entity/Transform.cpp
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
		core/utility/ModelImporter.cpp
		core/utility/JobManager.cpp
		core/utility/JobManager.h
		core/utility/Random.cpp
		core/utility/Random.h

		core/serialization/Serializer.cpp
		core/serialization/Serializer.h
		core/serialization/Deserializer.cpp
		core/serialization/Deserializer.h)

set(SCRIPTING_FILES
		scripting/ScriptManager.cpp
		scripting/ScriptManager.h
 )

set(GL_FILES
		graphics/gl/GL_Core.h
		graphics/gl/GL_Renderer.cpp
 		graphics/gl/GL_Texture.cpp
		graphics/gl/GL_ShaderProgram.cpp
		graphics/gl/GL_Mesh.cpp
		graphics/gl/GL_Framebuffer.cpp
		graphics/gl/GL_Framebuffer.h
		graphics/gl/GL_Debug.cpp
		graphics/gl/GL_Debug.h
		graphics/gl/GL_MultisamplingFramebuffer.cpp
		graphics/gl/GL_MultisamplingFramebuffer.h)

add_library (${PROJECT_NAME}
		${CORE_FILES}
		${GL_FILES}
		${SCRIPTING_FILES}
		)

target_link_libraries(${PROJECT_NAME} glm glfw glad stb assimp lua)

