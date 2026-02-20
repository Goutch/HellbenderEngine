//
// Created by User on 3/22/2023.
//

#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/resource/RasterizationTarget.h"
#include "core/scene/ecs/Component.h"

namespace HBE{

	struct HB_API PixelCamera {
		COMPONENT_IDS(PixelCamera);
			PixelCamera() {};

			PixelCamera(const PixelCamera &other);

			bool active = true;
			mat4 projection = mat4(1.0f);
			uint32_t layer_mask = UINT32_MAX;

			event_subscription_id render_target_resize_subscription_id;
			float aspectRatio();

			void setRenderTarget(RasterizationTarget *render_target);

			RasterizationTarget *getRenderTarget();

			void setNearPlane(float near);

			void setFarPlane(float far);

			float getNearPlane();

			float getFarPlane();

			private:
			RasterizationTarget *render_target = nullptr;
			float near = -1000;
			float far = 1000;

			void calculateProjection(RasterizationTarget *render_target);
	};
}
