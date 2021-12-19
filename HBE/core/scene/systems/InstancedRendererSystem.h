
#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/Components.h"
#include "core/scene/System.h"

namespace HBE {
	class Scene;

	class HB_API InstancedRendererSystem : public System {
		std::unordered_map<GraphicPipeline *, std::unordered_map<Mesh *, std::vector<mat4>>> transform_batches;
	public:
		InstancedRendererSystem(Scene *scene);
		~InstancedRendererSystem();
		void draw();
	};
}

