#pragma once

#include "HBE.h"

using namespace HBE;
struct SimCamera2D {
	f32 zoom_ratio;
};

class SimCamera2DSystem : public System {

	~SimCamera2DSystem();
public:
	void update(float delta);
	void onAttachSimCamera(Entity entity);
	SimCamera2DSystem(Scene *scene);
};

