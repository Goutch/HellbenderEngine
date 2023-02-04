
#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/Components.h"
#include "core/scene/System.h"
#include "core/scene/Scene.h"

namespace HBE {
	class Scene;

	class SceneNode;

	class HB_API MeshRendererSystem : public System {
		component_type_id mesh_renderer_component_id;
		component_type_id transform_component_id;

	public:
		MeshRendererSystem(Scene *scene);

		~MeshRendererSystem();

		void draw();

	private :
		void drawNode(SceneNode &node);

	};
}

