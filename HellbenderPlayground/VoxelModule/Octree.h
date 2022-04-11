#pragma once

#include "vector"

#define LEFT 0
#define RIGHT 1
#define BOTTOM 0
#define TOP 2
#define BACK 0
#define FRONT 4


class Octree {

	struct branch {
		uint8_t valid_mask;
		uint8_t value;
	//	uint32 child;
	};

	std::vector<branch> child_descriptor;
	std::vector<uint8_t> leafs;
	std::vector<uint32_t> trees;

	Octree(uint32_t size, uint8_t data);

};



