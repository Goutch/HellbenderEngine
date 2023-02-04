#include <glm/ext/matrix_transform.hpp>
#include "Components.h"
#include "glm/gtc/quaternion.hpp"

#include <core/resource/Model.h>
#include <core/resource/Mesh.h>
#include <core/resource/GraphicPipelineInstance.h>
#include <core/resource/RenderTarget.h>
#include <core/resource/Resources.h>
#include <vector>
#include "Application.h"
#include "core/scene/Scene.h"

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
		vec3 s = scale();
		vec3 pos = position();
		local_mat = mat4(1.0f);
		rotate(rot);
		setPosition(pos);
		setScale(s);
	}

	void Transform::setRotation(vec3 rot) {
		vec3 s = scale();
		vec3 pos = position();
		local_mat = mat4(1.0f);
		rotate(rot);
		setPosition(pos);
		setScale(s);
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

	const mat4& Transform::world() {
		Entity parent = Application::getScene()->getParent(entity);
		if (parent.valid() && parent.has<Transform>()) {
			if (is_dirty) {
				is_dirty = false;
				world_mat = parent.get<Transform>().world() * local_mat;
			}
			return world_mat;
		}
		return local_mat;
	}

	quat Transform::rotation() const {
		return glm::quat_cast(local_mat);
	}

	quat Transform::worldRotation() {
		Entity parent = Application::getScene()->getParent(entity);
		if (parent.valid() && parent.has<Transform>()) {

			return rotation() * parent.get<Transform>().worldRotation();
		}
		return rotation();
	}

	vec3 Transform::eulerRotation() const {
		return glm::eulerAngles(rotation());
	}

	vec3 Transform::worldEulerRotation() {
		return glm::eulerAngles(worldRotation());
	}

	void Transform::setScale(vec3 s) {
		is_dirty = true;
		local_mat = glm::scale(local_mat, vec3(1, 1, 1) / scale());
		local_mat = glm::scale(local_mat, s);
	}

	vec3 Transform::scale() const {

		return vec3(glm::length(local_mat[0]),
					glm::length(local_mat[1]),
					glm::length(local_mat[2]));

	}

	void Transform::setLocal(const mat4 &local) {
		is_dirty = true;
		local_mat = local;
	}

	const mat4& Transform::local() const {
		return local_mat;
	}


	//---------------------------------------CAMERA---------------------------------------
	void Camera::calculateProjection() {
		projection = glm::perspective<float>(glm::radians(fov), aspectRatio(), near, render_distance);
		projection[1] = -projection[1];
	}

	float Camera::aspectRatio() {
		vec2i res = render_target->getResolution();
		return static_cast<float>(res.x) / static_cast<float>(res.y);
	}


//------------------------------------TRANSFORM2D------------------------------------------------------
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


	void Camera2D::calculateProjection() {
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
}
