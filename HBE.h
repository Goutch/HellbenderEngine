#pragma once
#define _USE_MATH_DEFINES

#include "Core.h"
#include "Configs.h"
#include "Application.h"
#include "Core.h"
#include "HBETypes.h"

#include "core/resource/Resources.h"
#include "core/resource/Shader.h"
#include "core/resource/RasterizationPipeline.h"
#include "core/resource/ComputePipeline.h"
#include "core/resource/ComputeInstance.h"
#include "core/resource/Mesh.h"
#include "core/resource/Image.h"
#include "core/resource/Model.h"
#include "core/resource/RasterizationTarget.h"
#include "core/resource/RasterizationPipelineInstance.h"
#include "core/resource/ComputeInstance.h"
#include "core/resource/StorageBuffer.h"
#include "core/resource/raytracing/AccelerationStructure.h"
#include "core/resource/raytracing/RaytracingPipeline.h"
#include "core/resource/raytracing/RaytracingPipelineInstance.h"
#include "core/resource/AudioClip.h"
#include "core/resource/AudioClipInstance.h"
#include "core/resource/TexelBuffer.h"

#include "core/scene/Entity.h"
#include "core/scene/Scene.h"
#include "core/scene/System.h"
#include "core/scene/components/EntityState.h"
#include "core/scene/components/ModelRenderer.h"
#include "core/scene/components/Camera.h"
#include "core/scene/components/Camera2D.h"
#include "core/scene/components/PixelCamera.h"
#include "core/scene/components/CameraController.h"
#include "core/scene/components/Transform2D.h"
#include "core/scene/components/Transform.h"
#include "core/scene/systems/ui/UIPanelSystem.h"
#include "core/scene/systems/ui/ButtonSystem.h"
#include "core/scene/systems/ui/TextBoxSystem.h"
#include "core/scene/systems/ui/LabelSystem.h"

#include "core/graphics/Graphics.h"
#include "core/graphics/Window.h"
#include "core/graphics/RenderGraph.h"
#include "core/resource/RasterizationTarget.h"

#include "core/input/Input.h"
#include "core/math/Plane.h"

#include "core/threading/Job.h"

#include "core/utility/Log.h"
#include "core/utility/Geometry.h"
#include "core/utility/Profiler.h"
#include "core/utility/DefaultModelParser.h"
#include "Random.h"
#include "Event.h"
#include "Timer.h"


#include <glm/glm.hpp>
#include "glm/gtx/quaternion.hpp"

#include "core/math/fixed.h"


using namespace utils;