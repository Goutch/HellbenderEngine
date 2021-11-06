#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/resource/Mesh.h"
#include "core/resource/GraphicPipeline.h"
#include "core/resource/RenderTarget.h"
namespace HBE {
	struct Identity {
		std::string name = "";
	};
	struct Transform {
		mat4 matrix = mat4(1.0f);
	};
	struct Camera {
		mat4 projection = mat4(1.0f);
		RenderTarget* render_target;
	};
	struct MeshRenderer {
		Mesh *mesh;
		GraphicPipeline *pipeline;
	};
}
