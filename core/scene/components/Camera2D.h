#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/resource/RenderTarget.h"
namespace HBE
{
	struct HB_API Camera2D {
		Camera2D() {};

		Camera2D(const Camera2D &other);

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

		void setRenderTarget(RenderTarget *render_target);

		RenderTarget *getRenderTarget();

	private:
		RenderTarget *render_target = nullptr;
		float zoom_ratio = 10.0f;
		float near = -1000;
		float far = 1000;

		void calculateProjection(RenderTarget *render_target);
	};

}