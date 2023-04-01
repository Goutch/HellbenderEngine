#include "Transform2D.h"
namespace HBE{
	mat3 Transform2D::world() const {
		return parent == nullptr ? local : parent->world() * local;
	}

	void Transform2D::translate(vec2 translation) {
		local[2] += vec3(translation, 1.0f);
	}

	vec2 Transform2D::position() {
		return local[2];
	}

	float Transform2D::rotation() const {
		return atan2(local[0].x, local[0].y);
	}

	vec2 Transform2D::worldPosition() const {
		return local[2];
	}

	void Transform2D::setPosition(vec2 pos) {
		local[2].x = pos.x;
		local[2].y = pos.y;
	}

	void Transform2D::rotate(float rot) {
		mat3 rotation_mat = mat3(1.0f);
		rotation_mat[0].x = cos(rot);
		rotation_mat[0].y = sin(rot);
		rotation_mat[1].x = -rotation_mat[0].y;
		rotation_mat[1].y = rotation_mat[0].x;
		local *= rotation_mat;
	}

	void Transform2D::setRotaton(float rot) {
		local[0].x = cos(rot);
		local[0].y = sin(rot);
		local[1].x = -local[0].y;
		local[1].y = local[0].x;
	}

	float Transform2D::worldRotation() const {
		mat3 world_mat = world();
		return atan2(world_mat[0].x, world_mat[0].y);
	}

	void Transform2D::setScale(vec2 s) {
		local[0] *= s.y;
		local[1] *= s.x;
	}

	vec2 Transform2D::scale() const {
		return vec2(glm::length(local[0]),
					glm::length(local[1]));

	}
}