#pragma once

#include "Core.h"
#include "core/scene/Entity.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/glm.hpp"
#include "Event.h"

using namespace glm;
using namespace utils;

namespace HBE
{
	struct HB_API Transform
	{
		friend class TransformSystem;
		friend class Scene;
		friend class HierarchySystem;

	private:
		Entity entity;
		mat4 world_mat = mat4(1.0f);
		mat4 local_mat = mat4(1.0f);
		bool is_dirty = false;
	public:

		const mat4& local() const;

		void setDirty();

		void setLocal(const mat4& local);

		const mat4& world();

		void translate(vec3 translation);

		vec3 position() const;

		vec3 worldPosition();

		void setPosition(vec3 pos);

		quat rotation() const;

		void rotate(quat rot);

		void rotate(vec3 rot);

		void setRotation(quat rot);

		void setRotation(vec3 rot);

		void setLocalScale(vec3 s);

		vec3 localScale() const;

		void setWorldScale(vec3 s);

		vec3 worldScale();

		vec3 eulerRotation() const;

		quat worldRotation();

		vec3 worldEulerRotation();

		vec3 worldDown();

		vec3 worldUp();

		vec3 worldLeft();

		vec3 worldRight();

		vec3 worldBackward();

		vec3 worldForward();

		vec3 down() const;

		vec3 up() const;

		vec3 back() const;

		vec3 right() const;

		vec3 front() const;

		vec3 left() const;
	};
}
