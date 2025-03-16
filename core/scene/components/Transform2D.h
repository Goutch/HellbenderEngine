#pragma once

#include <core/resource/raytracing/AccelerationStructure.h>
#include <core/resource/raytracing/AccelerationStructure.h>

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/Entity.h"

namespace HBE
{
	struct HB_API Transform2D
	{
	private:
		Entity entity;
		mat3 local_mat = mat3(1.0f);
		mat3 world_mat = mat3(1.0f);
		bool is_dirty = true;

	public:
		void setDirty();
		mat3& world();

		vec2 worldPosition();

		void translate(vec2 translation);

		vec2 position();

		void setPosition(vec2 pos);

		float rotation() const;

		void rotate(float rotation);

		void setRotaton(float rotation);

		float worldRotation();

		void setScale(vec2 s);

		vec2 scale() const;
	};
}
