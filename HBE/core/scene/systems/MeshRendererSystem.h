
#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/Components.h"
#include "core/scene/System.h"
namespace HBE{
	class Scene;
	class HB_API MeshRendererSystem :public System{

	public:
		MeshRendererSystem(Scene* scene);
		~MeshRendererSystem();
		void draw();
	};
}

