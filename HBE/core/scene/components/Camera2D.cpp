#include "Camera2D.h"
#include "glm/gtc/matrix_transform.hpp"
namespace HBE{
	void Camera2D::calculateProjection(RenderTarget *render_target) {
		float aspect_ratio = aspectRatio();
		if (zoom_ratio < 0.1)
			zoom_ratio = 0.1;
		projection = glm::ortho(zoom_ratio * -0.5f * aspect_ratio, zoom_ratio * 0.5f * aspect_ratio, zoom_ratio * 0.5f, zoom_ratio * -.5f, near, far);
		//projection[1] = -projection[1];
	}

	float Camera2D::aspectRatio() {
		vec2i res = render_target->getResolution();
		return static_cast<float>(res.x) / static_cast<float>(res.y);
	}

	Camera2D::Camera2D(const Camera2D &other) {
		setRenderTarget(other.render_target);
	}

	void Camera2D::setRenderTarget(RenderTarget *render_target) {
		if (render_target != nullptr)
			render_target->onResolutionChange.unsubscribe(this);
		this->render_target = render_target;

		render_target->onResolutionChange.subscribe(this, &Camera2D::calculateProjection);
		calculateProjection(render_target);
	}

	RenderTarget *Camera2D::getRenderTarget() {
		return render_target;
	}

	void Camera2D::setNearPlane(float near) {
		this->near = near;
		calculateProjection(render_target);
	}

	void Camera2D::setFarPlane(float far) {
		this->far = far;
		calculateProjection(render_target);
	}

	float Camera2D::getNearPlane() {
		return near;
	}

	float Camera2D::getFarPlane() {
		return far;
	}

	float Camera2D::getZoomRatio() {
		return zoom_ratio;
	}

	void Camera2D::setZoomRatio(float ratio) {
		zoom_ratio = ratio;
		calculateProjection(render_target);
	}
}
