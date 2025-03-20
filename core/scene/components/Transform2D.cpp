#include "Transform2D.h"
#include <core/scene/Scene.h>
#include "HierarchyNode.h"

namespace HBE
{
	struct HierarchyNode;

	void Transform2D::setDirty()
	{
		is_dirty = true;
		if (entity.has<HierarchyNode>())
		{
			HierarchyNode* node = entity.get<HierarchyNode>();

			for (entity_handle child : node->children)
			{
				Transform2D* child_transform = entity.getScene()->get<Transform2D>(child);
				child_transform->setDirty();
			}
		}
	}

	mat3& Transform2D::world()
	{
		if (entity.has<HierarchyNode>())
		{
			HierarchyNode* node = entity.get<HierarchyNode>();
			if (entity.getScene()->valid(node->parent))
			{
				Transform2D* parent = entity.getScene()->get<Transform2D>(node->parent);
				if (is_dirty)
				{
					is_dirty = false;
					world_mat = parent->world() * local_mat;
				}
				return world_mat;
			}
		}

		return local_mat;
	}

	void Transform2D::translate(vec2 translation)
	{
		local_mat[2] += vec3(translation, 1.0f);
		setDirty();
	}

	vec2 Transform2D::position()
	{
		return local_mat[2];
	}

	float Transform2D::rotation() const
	{
		return atan2(local_mat[0].x, local_mat[0].y);
	}

	vec2 Transform2D::worldPosition()
	{
		const mat3& world_mat = world();
		return world_mat[2];
	}

	void Transform2D::setPosition(vec2 pos)
	{
		local_mat[2].x = pos.x;
		local_mat[2].y = pos.y;
		setDirty();
	}

	void Transform2D::rotate(float rot)
	{
		mat3 rotation_mat = mat3(1.0f);
		rotation_mat[0].x = cos(rot);
		rotation_mat[0].y = sin(rot);
		rotation_mat[1].x = -rotation_mat[0].y;
		rotation_mat[1].y = rotation_mat[0].x;
		local_mat *= rotation_mat;
		setDirty();
	}

	void Transform2D::setRotaton(float rot)
	{
		local_mat[0].x = cos(rot);
		local_mat[0].y = sin(rot);
		local_mat[1].x = -local_mat[0].y;
		local_mat[1].y = local_mat[0].x;
		setDirty();
	}

	float Transform2D::worldRotation()
	{
		const mat3& world_mat = world();
		return atan2(world_mat[0].x, world_mat[0].y);
	}

	void Transform2D::setScale(vec2 s)
	{
		local_mat[0] *= s.y;
		local_mat[1] *= s.x;
		setDirty();
	}

	vec2 Transform2D::scale() const
	{
		return vec2(glm::length(local_mat[0]),
		            glm::length(local_mat[1]));
	}
}
