#pragma once

#include "HBE.h"
#include "Pathfinder/Map.h"
using namespace HBE;

class PlayerInputSystem : public System {
	Entity camera;
	Map& map;
public:

	PlayerInputSystem(Map& map,Scene *scene);
	~PlayerInputSystem();
	void update(float delta);
};

