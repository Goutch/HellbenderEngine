#pragma once

#include "HBETypes.h"

namespace HBE {
	inline float sdBox(vec3 point, vec3 box_min, vec3 box_max) {
		vec3 radius = (box_max - box_min) * 0.5f;
		vec3 center = box_min + radius;
		vec3 q = abs(point - center) - radius;
		return length(glm::max(q, vec3(0.0f))) + glm::min(glm::max(q.x, q.y), 0.0f);
	}

	inline float sdBox(vec2 point, vec2 box_min, vec2 box_max) {
		vec2 radius = (box_max - box_min) * 0.5f;
		vec2 center = box_min + radius;
		vec2 q = abs(point - center) - radius;
		return length(glm::max(q, vec2(0.0f))) + glm::min(glm::max(q.x, q.y), 0.0f);
	}
}