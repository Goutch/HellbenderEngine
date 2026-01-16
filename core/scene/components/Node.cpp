//
// Created by username on 3/7/2025.
//

#include "Node.h"
#include "core/scene/Scene.h"

namespace HBE
{
	Node::Node()
	{
	}

	Node::Node(Entity entity)
	{
		this->entity = entity;
	}

	Node::Node(Node&& other) noexcept
	{
		parent = other.parent;
		children = std::move(other.children);
	}

	uint32_t Node::getDepth() const
	{
		return depth;
	}

	uint32_t Node::getLocalIndex() const
	{
		return local_index;
	}

	uint32_t Node::getGlobalIndex() const
	{
		return global_index;
	}

	bool Node::hasParent() const
	{
		return parent != NULL_ENTITY_HANDLE;
	}

	bool Node::isActive() const
	{
		return is_active;
	}

	bool Node::isActiveInHierarchy()
	{
		if (!isActive()) return false;
		if (active_in_hierarchy_dirty)
		{
			if (entity.getScene()->valid(parent))
			{
				Node* parent_node = entity.getScene()->get<Node>(this->parent);
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
