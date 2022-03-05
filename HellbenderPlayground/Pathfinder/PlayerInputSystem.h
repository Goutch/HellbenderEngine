#pragma once

#include "HBE.h"

using namespace HBE;


class PlayerInputSystem : public System {

	Entity camera;
public:

	PlayerInputSystem(Scene *scene);
	~PlayerInputSystem();
	void update(float delta);
};

