//
// Created by username on 3/6/2025.
//

#include "HierarchySystem.h"

#include <core/scene/Scene.h>

#include "core/scene/components/HierachyNode.h"
#include "core/scene/ecs/ComponentPool.h"

namespace HBE
{
	HierarchySystem::HierarchySystem(Scene* scene): System(scene)
	{
		scene->onAttach<HierarchyNode>().subscribe(this, &HierarchySystem::onAttachNode);
		scene->onDetach<HierarchyNode>().subscribe(this, &HierarchySystem::onDetachNode);
	}

	void HierarchySystem::updateNodeIndices()
	{
		if (dirty)
			updateNodeIndices(root_nodes, 0, 0);
	}

	void HierarchySystem::onDetachNode(Entity entity)
	{
		HierarchyNode* node = entity.get<HierarchyNode>();

		if (node->hasParent())
		{
			HierarchyNode* parent_node = scene->get<HierarchyNode>(node->parent);
			parent_node->children.erase(parent_node->children.begin() + node->getLocalIndex());
		}
		else
		{
			root_nodes.erase(root_nodes.begin() + node->getLocalIndex());
		}

		node_count--;
		dirty = true;
	}

	void HierarchySystem::setParent(entity_handle entity, entity_handle parent)
	{
		HierarchyNode* node = scene->get<HierarchyNode>(entity);
		HierarchyNode* old_parent_node = node->hasParent() ? scene->get<HierarchyNode>(node->parent) : nullptr;
		HierarchyNode* new_parent_node = scene->valid(parent) ? scene->get<HierarchyNode>(parent) : nullptr;
		if (old_parent_node == new_parent_node)
			return;
		std::vector<entity_handle>& source = old_parent_node != nullptr ? old_parent_node->children : root_nodes;
		std::vector<entity_handle>& destination = new_parent_node != nullptr ? new_parent_node->children : root_nodes;

		if (destination == source)
			return;

		node->parent = parent;

		source.erase(source.begin() + node->getLocalIndex());
		destination.push_back(entity);

		node->local_index = destination.size() - 1;

		if (scene->has<Transform>(entity))
		{
			Transform* transform = scene->get<Transform>(entity);
			transform->setDirty();
		}
		setDirty(node);
		dirty = true;
	}

	void HierarchySystem::setDirty(HierarchyNode* node)
	{
		node->active_in_hierarchy_dirty = true;
		for (entity_handle child : node->children)
		{
			setDirty(scene->get<HierarchyNode>(child));
		}
	}

	void HierarchySystem::setParent(Entity entity, Entity parent)
	{
		setParent(entity.getHandle(), parent.getHandle());
	}

	std::vector<entity_handle>& HierarchySystem::getRootNodes()
	{
		return root_nodes;
	}


	void HierarchySystem::onAttachNode(Entity entity)
	{
		HierarchyNode* node = entity.get<HierarchyNode>();
		if (node->parent == NULL_ENTITY_HANDLE)
		{
			root_nodes.push_back(entity.getHandle());
			node->local_index = root_nodes.size() - 1;
			node->depth = 0;
		}
		else
		{
			HierarchyNode* parent_node = scene->get<HierarchyNode>(node->parent);
			parent_node->children.push_back(entity.getHandle());
			node->local_index = parent_node->children.size() - 1;
			node->depth = parent_node->getDepth() + 1;
		}
		dirty = true;
		node_count++;
	}

	void HierarchySystem::updateNodeIndices(std::vector<entity_handle> nodes, uint32_t global_index, uint32_t depth)
	{
		uint32_t local_index = 0;
		for (entity_handle node_entity : nodes)
		{
			HierarchyNode* node = scene->get<HierarchyNode>(node_entity);
			updateNodeIndices(node->children, global_index, depth + 1);

			node->local_index = local_index;
			node->global_index = global_index;
			global_index++;
			local_index++;
		}
	}
}
