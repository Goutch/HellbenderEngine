#pragma once
#include <Core.h>
#include <Configs.h>
#include <Application.h>

#include <core/resource/ShaderProgram.h>
#include <core/resource/Mesh.h>
#include <core/resource/Texture.h>
#include <core/resource/Material.h>
#include <core/resource/Model.h>

#include <core/entity/Entity.h>
#include <core/entity/Scene.h>

#include <core/entity/component/Transform.h>
#include <core/entity/component/Component.h>
#include <core/entity/component/MeshRenderer.h>
#include <core/entity/component/InstancesRenderer.h>
#include <core/entity/component/Camera.h>
#include <core/entity/component/CameraController.h>
#include <core/entity/component/ModelRenderer.h>


#include <core/graphics/Graphics.h>
#include <core/graphics/RenderTarget.h>
#include <core/input/Input.h>
#include <core/math/Plane.h>

#include <core/scripting/ScriptManager.h>

#include <core/utility/Event.h>
#include <core/utility/Clock.h>
#include <core/utility/JobManager.h>
#include <core/utility/Log.h>
#include <core/utility/Geometry.h>
#include <core/utility/Random.h>

#include <core/serialization/Serializer.h>
#include <core/serialization/Deserializer.h>
#include <core/serialization/Serializable.h>
#include <core/serialization/ComponentRegistry.h>

#include <glm/glm.hpp>
#include "glm/gtx/quaternion.hpp"

using namespace glm;