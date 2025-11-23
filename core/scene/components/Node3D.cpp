//
// Created by username on 3/7/2025.
//

#include "Node3D.h"
#include "core/scene/Scene.h"

namespace HBE
{
	Node3D::Node3D()
	{
	}

	Node3D::Node3D(Entity entity)
	{
		this->entity = entity;
	}

	Node3D::Node3D(Node3D&& other) noexcept
	{
		parent = other.parent;
		children = std::move(other.children);
	}

	uint32_t Node3D::getDepth() const
	{
		return depth;
	}

	uint32_t Node3D::getLocalIndex() const
	{
		return local_index;
	}

	uint32_t Node3D::getGlobalIndex() const
	{
		return global_index;
	}

	bool Node3D::hasParent() const
	{
		return parent != NULL_ENTITY_HANDLE;
	}

	bool Node3D::isActive() const
	{
		return is_active;
	}

	bool Node3D::isActiveInHierarchy()
	{
		if (!isActive()) return false;
		if (active_in_hierarchy_dirty)
		{
			if (entity.getScene()->valid(parent))
			{
				Node3D* parent_node = entity.getScene()->get<Node3D>(this->parent);
				if (parent_node == nullptr)
				{
					is_active_in_hierarchy = true;
				}

				is_active_in_hierarchy = parent_node->isActiveInHierarchy();
			}
			else
			{
				is_active_in_hierarchy = true;
			}
			active_in_hierarchy_dirty = false;
		}
		return is_active_in_hierarchy;
	}
}
