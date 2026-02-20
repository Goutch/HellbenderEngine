#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/resource/RasterizationTarget.h"
#include "core/scene/ecs/Component.h"

namespace HBE
{
	struct HB_API Camera2D {
		COMPONENT_IDS(Camera2D)
		Camera2D() {};

		Camera2D(const Camera2D &other);

		event_subscription_id render_target_resize_subscription_id ;
		bool active = true;
		mat4 projection = mat4(1.0f);

		uint32_t layer_mask = UINT32_MAX;

		void setNearPlane(float near);

		void setFarPlane(float far);

		float getNearPlane();

		float getFarPlane();

		float getZoomRatio();

		void setZoomRatio(float ratio);

		float aspectRatio();

		void setRenderTarget(RasterizationTarget *render_target);

		RasterizationTarget *getRenderTarget();

	private:
		RasterizationTarget *render_target = nullptr;
		float zoom_ratio = 10.0f;
		float near = -1000;
		float far = 1000;

		void calculateProjection(RasterizationTarget *render_target);
	};

}
