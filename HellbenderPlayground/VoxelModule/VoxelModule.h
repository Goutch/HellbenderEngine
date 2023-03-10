#pragma once

#include "HBE.h"
#include "VoxelRendererSystem.h"

using namespace HBE;

class VoxelModule {
public :
	static void init(Scene& scene) {
		//-------------------SCENE CREATION--------------------------------------
		scene.addSystem(new VoxelRendererSystem(&scene));
	}
};
