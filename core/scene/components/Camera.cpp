#include "Camera.h"

namespace HBE {


	float Camera::aspectRatio() {
		vec2i res = render_target->getResolution();
		return static_cast<float>(res.x) / static_cast<float>(res.y);
	}

	void Camera::calculateProjection(RasterizationTarget *render_target) {
		projection = glm::perspective<float>(glm::radians(fov), aspectRatio(), near, far);
		projection[1] = -projection[1];
	}

	void Camera::setRenderTarget(RasterizationTarget *render_target) {
		if (render_target != nullptr)
			render_target->onResolutionChange.unsubscribe(render_target_resize_subscription_id);
		this->render_target = render_target;

		render_target_resize_subscription_id = render_target->onResolutionChange.subscribe(this, &Camera::calculateProjection);
		calculateProjection(render_target);
	}

	RasterizationTarget *Camera::getRenderTarget() {
		return render_target;
	}

	Camera::Camera(const Camera &other) {
		setRenderTarget(other.render_target);
	}

	void Camera::setFOV(float fov) {
		this->fov = fov;
		calculateProjection(render_target);
	}

	float Camera::getFOV() {
		return fov;
	}

	void Camera::setNearPlane(float near) {
		this->near = near;
		calculateProjection(render_target);
	}

	void Camera::setFarPlane(float far) {
		this->far = far;
		calculateProjection(render_target);
	}

	float Camera::getNearPlane() {
		return near;
	}

	float Camera::getFarPlane() {
		return far;
	}
}
