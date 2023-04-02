//
// Created by User on 3/22/2023.
//

#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/resource/RenderTarget.h"
namespace HBE{
	struct HB_API PixelCamera {
			PixelCamera() {};

			PixelCamera(const PixelCamera &other);

			bool active = true;
			mat4 projection = mat4(1.0f);
			uint32_t layer_mask = UINT32_MAX;

			float aspectRatio();

			void setRenderTarget(RenderTarget *render_target);

			RenderTarget *getRenderTarget();

			void setNearPlane(float near);

			void setFarPlane(float far);

			float getNearPlane();

			float getFarPlane();

			private:
			RenderTarget *render_target = nullptr;
			float near = -1000;
			float far = 1000;

			void calculateProjection(RenderTarget *render_target);
	};
}