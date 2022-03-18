
#include "Map.h"

Map::Map(uint32_t size_x, uint32_t size_y) {

	this->size_x = size_x;
	this->size_y = size_y;
	walkable_map = new bool[size_x * size_y]{false};
	navmesh = new Navmesh(walkable_map, size_x, size_y, f32(1));
}

Map::~Map() {
	delete walkable_map;
	delete navmesh;
}

void Map::set(int x, int y, bool value) {
	if (x < size_x && x > 0 && y < size_y && y > 0) {
		walkable_map[x + (y * size_y)] = value;
		navmesh->update();
	}
}


