#include "Transform.h"

namespace HBE {
	void Transform::translate(vec3 translation) {
		is_dirty = true;
		local_mat = glm::translate(local_mat, translation);
	}

	vec3 Transform::position() const {
		return local_mat[3];
	}

	vec3 Transform::worldPosition() {
		mat4 mat_world = world();
		return vec3(mat_world[3].x, mat_world[3].y, mat_world[3].z);
	}

	vec3 Transform::front() const {
		return glm::normalize(local_mat[2]);
	}

	vec3 Transform::back() const {
		return -glm::normalize(local_mat[2]);
	}

	vec3 Transform::right() const {
		return glm::normalize(local_mat[0]);
	}

	vec3 Transform::left() const {
		return -glm::normalize(local_mat[0]);
	}

	vec3 Transform::up() const {
		return glm::normalize(local_mat[1]);
	}

	vec3 Transform::down() const {
		return -glm::normalize(local_mat[1]);
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
		vec3 s = localScale();
		vec3 pos = position();
		local_mat = mat4(1.0f);
		rotate(rot);
		setPosition(pos);
		setLocalScale(s);
	}

	void Transform::setRotation(vec3 rot) {
		vec3 s = localScale();
		vec3 pos = position();
		local_mat = mat4(1.0f);
		rotate(rot);
		setPosition(pos);
		setLocalScale(s);
	}

	void Transform::rotate(quat rot) {
		is_dirty = true;
		local_mat *= glm::mat4_cast(rot);
	}

	void Transform::rotate(vec3 rot) {
		rotate(quat(rot));
	}

	void Transform::setPosition(vec3 pos) {
		is_dirty = true;
		local_mat[3].x = pos.x;
		local_mat[3].y = pos.y;
		local_mat[3].z = pos.z;
	}

	const mat4 &Transform::world() {

		if (parent != nullptr) {
			if (is_dirty) {
				is_dirty = false;
				world_mat = parent->world() * local_mat;
			}
			return world_mat;
		}
		return local_mat;
	}

	quat Transform::rotation() const {
		return glm::quat_cast(local_mat);
	}

	quat Transform::worldRotation() {
		if (parent != nullptr) {

			return rotation() * parent->worldRotation();
		}
		return rotation();
	}

	vec3 Transform::eulerRotation() const {
		return glm::eulerAngles(rotation());
	}

	vec3 Transform::worldEulerRotation() {
		return glm::eulerAngles(worldRotation());
	}

	void Transform::setLocalScale(vec3 s) {
		is_dirty = true;
		local_mat = glm::scale(local_mat, vec3(1, 1, 1) / localScale());
		local_mat = glm::scale(local_mat, s);
	}

	vec3 Transform::localScale() const {
		return vec3(glm::length(local_mat[0]),
					glm::length(local_mat[1]),
					glm::length(local_mat[2]));

	}

	void Transform::setLocal(const mat4 &local) {
		is_dirty = true;
		local_mat = local;
	}

	const mat4 &Transform::local() const {
		return local_mat;
	}

	void Transform::setWorldScale(vec3 s) {
		setLocalScale(s / worldScale());
	}

	vec3 Transform::worldScale() {
		const mat4 &world_mat = world();
		return vec3(glm::length(world_mat[0]),
					glm::length(world_mat[1]),
					glm::length(world_mat[2]));
	}
}