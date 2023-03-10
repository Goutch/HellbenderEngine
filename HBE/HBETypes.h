#pragma once

#include "glm/glm.hpp"

namespace HBE {

	typedef uint64_t uint64;
	typedef uint32_t uint32;
	typedef uint16_t uint16;
	typedef uint8_t uint8;

	typedef int64_t int64;
	typedef int32_t int32;
	typedef int16_t int16;
	typedef int8_t int8;


	typedef glm::mat2 mat2;
	typedef glm::mat3 mat3;
	typedef glm::mat4 mat4;

	typedef glm::vec2 vec2;
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;

	typedef glm::ivec2 ivec2;
	typedef glm::ivec3 ivec3;
	typedef glm::ivec4 ivec4;

	typedef glm::uvec2 uvec2;
	typedef glm::uvec3 uvec3;
	typedef glm::uvec4 uvec4;

	typedef glm::vec<2, int32_t> vec2i;
	typedef glm::vec<3, int32_t> vec3i;
	typedef glm::vec<4, int32_t> vec4i;
	typedef glm::vec<2, uint32_t> vec2u;
	typedef glm::vec<3, uint32_t> vec3u;
	typedef glm::vec<4, uint32_t> vec4u;

	typedef glm::quat quat;

	enum TEXT_ALIGNMENT {
		TEXT_ALIGNMENT_LEFT,
		TEXT_ALIGNMENT_CENTER,
		TEXT_ALIGNMENT_RIGHT
	};
	typedef uint32_t AXIS_PIVOT_FLAGS;
	enum AXIS_PIVOT_FLAG {
		AXIS_PIVOT_FLAG_LEFT = 0,
		AXIS_PIVOT_FLAG_RIGHT = 1,
		AXIS_PIVOT_FLAG_TOP = 2,
		AXIS_PIVOT_FLAG_BOTTOM = 4,
		AXIS_PIVOT_FLAG_X_CENTER = 8,
		AXIS_PIVOT_FLAG_Y_CENTER = 16,
	};
	enum PIVOT {
		PIVOT_TOP_LEFT = AXIS_PIVOT_FLAG_LEFT | AXIS_PIVOT_FLAG_TOP,
		PIVOT_TOP_CENTER = AXIS_PIVOT_FLAG_X_CENTER | AXIS_PIVOT_FLAG_TOP,
		PIVOT_TOP_RIGHT = AXIS_PIVOT_FLAG_RIGHT | AXIS_PIVOT_FLAG_TOP,
		PIVOT_CENTER_LEFT = AXIS_PIVOT_FLAG_LEFT | AXIS_PIVOT_FLAG_Y_CENTER,
		PIVOT_CENTER = AXIS_PIVOT_FLAG_X_CENTER | AXIS_PIVOT_FLAG_Y_CENTER,
		PIVOT_CENTER_RIGHT = AXIS_PIVOT_FLAG_RIGHT | AXIS_PIVOT_FLAG_Y_CENTER,
		PIVOT_BOTTOM_LEFT = AXIS_PIVOT_FLAG_LEFT | AXIS_PIVOT_FLAG_BOTTOM,
		PIVOT_BOTTOM_CENTER = AXIS_PIVOT_FLAG_X_CENTER | AXIS_PIVOT_FLAG_BOTTOM,
		PIVOT_BOTTOM_RIGHT = AXIS_PIVOT_FLAG_RIGHT | AXIS_PIVOT_FLAG_BOTTOM,
	};
}
