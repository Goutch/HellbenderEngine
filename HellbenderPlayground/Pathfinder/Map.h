#pragma once

#include "HBE.h"

using namespace HBE;

#include "Navmesh.h"

class Map {
	uint32_t size_x;
	uint32_t size_y;
	bool *walkable_map;
public:
	Navmesh *navmesh = nullptr;

	Map(uint32_t size_x, uint32_t size_y);
	~Map();

	void set(int x, int y, bool value);
};

