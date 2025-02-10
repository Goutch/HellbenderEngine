//
// Created by User on 3/22/2023.
//

#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/resource/RenderTarget.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glm/gtc/quaternion.hpp"
namespace HBE {
	struct HB_API Camera {
		Camera() {};

		Camera(const Camera &other);

		mat4 projection = mat4(1.0f);
		float fov = 70.0f;
		float far = 1000.0f;
		float near = 0.1f;
		bool active = true;
		uint32_t layer_mask = UINT32_MAX;

		void setRenderTarget(RenderTarget *render_target);

		RenderTarget *getRenderTarget();

		float aspectRatio();

		void setFOV(float fov);

		float getFOV();

		void setNearPlane(float near);

		void setFarPlane(float far);

		float getNearPlane();

		float getFarPlane();

	private:
		RenderTarget *render_target = nullptr;

		void calculateProjection(RenderTarget *render_target);
	};

}