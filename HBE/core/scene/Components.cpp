#include <glm/ext/matrix_transform.hpp>
#include "Components.h"
#include "glm/gtc/quaternion.hpp"

namespace HBE {
	void Transform::translate(vec3 translation) {
		local = glm::translate(local, translation);
	}

	vec3 Transform::position() {
		return local[3];
	}

	vec3 Transform::front() {
		return glm::normalize(world()[2]);
	}

	vec3 Transform::back() {
		return -glm::normalize(world()[2]);
	}

	vec3 Transform::right() {
		return glm::normalize(world()[0]);
	}

	vec3 Transform::left() {
		return -glm::normalize(world()[0]);
	}

	vec3 Transform::up() {
		return glm::normalize(world()[1]);
	}

	vec3 Transform::down() {
		return -glm::normalize(world()[1]);
	}

	vec3 Transform::worldForward() {
		return glm::normalize(world()[2]);
	}

	vec3 Transform::worldBackward() {
		return -glm::normalize(world()[2]);
	}

	vec3 Transform::worldRight() {
		return glm::normalize(world()[0]);
	}

	vec3 Transform::worldLeft() {
		return -glm::normalize(world()[0]);
	}

	vec3 Transform::worldUp() {
		return glm::normalize(world()[1]);
	}

	vec3 Transform::worldDown() {
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
		local=glm::rotate(local, rot.x, vec3(1.f, 0.f, 0.f));
		local=glm::rotate(local, rot.y, vec3(0.f, 1.f, 0.f));
		local=glm::rotate(local, rot.z, vec3(0.f, 0.f, 1.f));

	}

	void Transform::setPosition(vec3 pos) {
		local[3].x = pos.x;
		local[3].y = pos.y;
		local[3].z = pos.z;
	}

	mat4 Transform::world() {
		return parent == nullptr ? local : parent->world() * local;
	}

	quat Transform::rotation() {
		return glm::quat_cast(local);
	}

	quat Transform::worldRotation() {
		return parent == nullptr ? rotation() : rotation() * parent->worldRotation();
	}

	vec3 Transform::eulerRotation() {
		return glm::eulerAngles(rotation());
	}

	vec3 Transform::worldEulerRotation() {
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

	vec3 Transform::scale() {

		return vec3(glm::length(local[0]),
					glm::length(local[1]),
					glm::length(local[2]));

	}

//---------------------------------------CAMERA---------------------------------------
	void Camera::calculateProjection() {
		projection = glm::perspective<float>(glm::radians(fov), aspectRatio(), near, render_distance);
	}

	float Camera::aspectRatio() {
		uint32_t w, h;
		render_target->getResolution(w, h);
		return static_cast<float>(w) / static_cast<float>(h);
	}
}
