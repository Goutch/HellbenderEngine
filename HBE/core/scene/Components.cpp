#include <glm/ext/matrix_transform.hpp>
#include "Components.h"
#include "glm/gtc/quaternion.hpp"

namespace HBE {
	void Transform::translate(vec3 translation) {
		local = glm::translate(local, translation);
	}

	vec3 Transform::position() const {
		return local[3];
	}

	vec3 Transform::front() const {
		return glm::normalize(world()[2]);
	}

	vec3 Transform::back() const {
		return -glm::normalize(world()[2]);
	}

	vec3 Transform::right() const {
		return glm::normalize(world()[0]);
	}

	vec3 Transform::left() const {
		return -glm::normalize(world()[0]);
	}

	vec3 Transform::up() const {
		return glm::normalize(world()[1]);
	}

	vec3 Transform::down() const {
		return -glm::normalize(world()[1]);
	}

	vec3 Transform::worldForward() const {
		return glm::normalize(world()[2]);
	}

	vec3 Transform::worldBackward() const {
		return -glm::normalize(world()[2]);
	}

	vec3 Transform::worldRight() const {
		return glm::normalize(world()[0]);
	}

	vec3 Transform::worldLeft() const {
		return -glm::normalize(world()[0]);
	}

	vec3 Transform::worldUp() const {
		return glm::normalize(world()[1]);
	}

	vec3 Transform::worldDown() const {
		return -glm::normalize(world()[1]);

	}

	void Transform::setRotation(quat rot) {
		vec3 s = scale();
		vec3 pos = position();
		local = mat4(1.0f);
		rotate(rot);
		setPosition(pos);
		setScale(s);
	}

	void Transform::setRotation(vec3 rot) {
		vec3 s = scale();
		vec3 pos = position();
		local = mat4(1.0f);
		rotate(rot);
		setPosition(pos);
		setScale(s);
	}

	void Transform::rotate(quat rot) {
		local *= glm::mat4_cast(rot);
	}

	void Transform::rotate(vec3 rot) {
		rotate(quat(rot));
	}

	void Transform::setPosition(vec3 pos) {
		local[3].x = pos.x;
		local[3].y = pos.y;
		local[3].z = pos.z;
	}

	mat4 Transform::world() const {
		return std::move(parent == nullptr ? local : parent->world() * local);
	}

	quat Transform::rotation() const {
		return glm::quat_cast(local);
	}

	quat Transform::worldRotation() const {
		return parent == nullptr ? rotation() : rotation() * parent->worldRotation();
	}

	vec3 Transform::eulerRotation() const {
		return glm::eulerAngles(rotation());
	}

	vec3 Transform::worldEulerRotation() const {
		return parent == nullptr ? glm::eulerAngles(rotation()) : glm::eulerAngles(glm::quat_cast(local) * parent->worldRotation());
	}

	void Transform::setScale(vec3 s) {
		quat rot = rotation();
		vec3 pos = position();
		local = mat4(1.0f);
		glm::scale(local, s);
		setPosition(pos);
		setRotation(rot);
	}

	vec3 Transform::scale() const {

		return vec3(glm::length(local[0]),
					glm::length(local[1]),
					glm::length(local[2]));

	}

//---------------------------------------CAMERA---------------------------------------
	void Camera::calculateProjection() {
		projection = glm::perspective<float>(glm::radians(fov), aspectRatio(), near, render_distance);
		projection[1] = -projection[1];
	}

	float Camera::aspectRatio() {
		uint32_t w, h;
		render_target->getResolution(w, h);
		return static_cast<float>(w) / static_cast<float>(h);
	}
}
