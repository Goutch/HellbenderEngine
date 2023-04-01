#include "PixelCamera.h"
#include "glm/gtc/matrix_transform.hpp"
namespace HBE{
	float PixelCamera::aspectRatio() {
		vec2i res = render_target->getResolution();
		return static_cast<float>(res.x) / static_cast<float>(res.y);
	}

	PixelCamera::PixelCamera(const PixelCamera &other) {
		setRenderTarget(other.render_target);
	}

	void PixelCamera::setRenderTarget(RenderTarget *render_target) {
		if (render_target != nullptr)
			render_target->onResolutionChange.unsubscribe(this);
		this->render_target = render_target;

		render_target->onResolutionChange.subscribe(this, &PixelCamera::calculateProjection);
		calculateProjection(render_target);

	}

	RenderTarget *PixelCamera::getRenderTarget() {
		return render_target;
	}

	void PixelCamera::calculateProjection(RenderTarget *render_target) {
		projection = glm::ortho(0.0f,
								static_cast<float>(render_target->getResolution().x),
								static_cast<float>(render_target->getResolution().y),
								0.0f,
								near, far);
	}

	void PixelCamera::setNearPlane(float near) {
		this->near = near;
		calculateProjection(render_target);
	}

	void PixelCamera::setFarPlane(float far) {
		this->far = far;
		calculateProjection(render_target);
	}

	float PixelCamera::getNearPlane() {
		return near;
	}

	float PixelCamera::getFarPlane() {
		return far;
	}
}
