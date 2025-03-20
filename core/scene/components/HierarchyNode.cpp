//
// Created by username on 3/7/2025.
//

#include "HierarchyNode.h"
#include "core/scene/Scene.h"

namespace HBE
{
	HierarchyNode::HierarchyNode()
	{
	}

	HierarchyNode::HierarchyNode(Entity entity)
	{
		this->entity = entity;
	}

	HierarchyNode::HierarchyNode(HierarchyNode&& other) noexcept
	{
		parent = other.parent;
		children = std::move(other.children);
	}

	uint32_t HierarchyNode::getDepth() const
	{
		return depth;
	}

	uint32_t HierarchyNode::getLocalIndex() const
	{
		return local_index;
	}

	uint32_t HierarchyNode::getGlobalIndex() const
	{
		return global_index;
	}

	bool HierarchyNode::hasParent() const
	{
		return parent != NULL_ENTITY_HANDLE;
	}

	bool HierarchyNode::isActive() const
	{
		return is_active;
	}

	bool HierarchyNode::isActiveInHierarchy()
	{
		if (!isActive()) return false;
		if (active_in_hierarchy_dirty)
		{
			if (entity.getScene()->valid(parent))
			{
				HierarchyNode* parent_node = entity.getScene()->get<HierarchyNode>(this->parent);
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
