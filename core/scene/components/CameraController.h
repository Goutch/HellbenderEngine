//
// Created by User on 3/22/2023.
//

#pragma once

#include "Core.h"
#include "HBETypes.h"

namespace HBE {
	struct HB_API CameraController {
		float speed = 1.0f;
		float sensibility = 1.0f;
		float current_pitch = 0.0f;
		float max_pitch = 90.0f;
		bool active = true;
		bool invert_y = false;
	};
}
