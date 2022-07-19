#pragma once

#include <cstdint>
#include <vector>
#include "string"
#include "unordered_map"
class VoxTree {
private:
	const static uint8_t[8]
	EMPTY_LEAF = {0, 0, 0, 0, 0, 0, 0, 0};
	const static uint32_t EMPTY_NODE_MASK = static_cast<uint32_t>(0x7FFFFFFFF);
	const static uint32_t NODE_DIMENSION = 2;
	const static uint8_t RIGHT;
	const static uint8_t UP;
	const static uint8_t FORWARD;
	const static uint8_t ALL_LEAF;

	//value: 00000(5bits unused)|Valid_MASK(27bit)
	//index: points to a leaf index 00000(5bits unused)|LEAF_MASK(27bit)
	struct Node {
		uint8_t valid_mask;
		uint16_t index;
	};
	struct Leaf {
		uint8_t[NODE_DIMENSION *NODE_DIMENSION*NODE_DIMENSION] values;
	};
	struct Signature{
		uint64_t value=0;
		uint16_t ref_count=0;
	};
	uint16_t dimensions;
public:
	std::vector<Node> nodes;
	std::vector<Leaf> leaves;
	std::unordered_map<Signature,uint16_t> signature_to_index;

	VoxTree(uint16_t dimensions) : dimensions(dimensions) {
		static_assert(dimensions % NODE_DIMENSION == 0, "dimensions must be a multiple of " + std::to_string(NODE_DIMENTSION));
		nodes.emplace_back(EMPTY_NODE_MASK, 0)
		leaves.emplace_back(EMPTY_LEAF);
	}

	void set(uint8_t value, uint16_t x, uint16_t y, uint16_t z) {
		uint8_t depth = 0;
		Node current_node = nodes[0];
		uint8_t valid_bit=(RIGHT*x)
		while (pow(depth, NODE_DIMENSION) != dimensions) {
			if (current_node.valid_mask&==1)
		}

		//leaf.values[(RIGHT * x) + (UP * y) + (z * FORWARD)] = value;
	}


	void remove(uint16_t x, uint16_t y, uint16_t z) {

	}


};