//
// Created by username on 3/6/2025.
//

#include "Node3DSystem.h"
#include "core/scene/components/Node3D.h"
#include <core/scene/Scene.h>
#include "core/scene/ecs/ComponentPool.h"

namespace HBE
{
	Node3DSystem::Node3DSystem(Scene* scene): System(scene)
	{
		scene->onAttach<Node3D>().subscribe(this, &Node3DSystem::onAttachNode);
		scene->onDetach<Node3D>().subscribe(this, &Node3DSystem::onDetachNode);
	}

	void Node3DSystem::updateNodeIndices()
	{
		if (dirty)
			updateNodeIndices(root_nodes, 0, 0);
	}

	void Node3DSystem::onDetachNode(Entity entity)
	{
		Node3D* node = entity.get<Node3D>();

	if (node->hasParent())
		{
			Node3D* parent_node = scene->get<Node3D>(node->parent);
			parent_node->children.erase(parent_node->children.begin() + node->getLocalIndex());
		}
		for (int i = node->getLocalIndex() + 1; i < root_nodes.size(); ++i) {
			scene->get<Node3D>(root_nodes[i])->local_index--;
		}
		root_nodes.erase(root_nodes.begin() + node->getLocalIndex());

		node_count--;
		dirty = true;
	}

void Node3DSystem::setParent(entity_handle entity, entity_handle parent)
	{
		Node3D* node = scene->get<Node3D>(entity);
		Node3D* old_parent_node = node->hasParent() ? scene->get<Node3D>(node->parent) : nullptr;
		Node3D* new_parent_node = scene->valid(parent) ? scene->get<Node3D>(parent) : nullptr;
		if (old_parent_node == new_parent_node)
			return;
		std::vector<entity_handle> &source = old_parent_node != nullptr ? old_parent_node->children : root_nodes;
		std::vector<entity_handle> &destination = new_parent_node != nullptr ? new_parent_node->children : root_nodes;

		if (destination == source)
			return;

		node->parent = parent;
		for (int i = node->local_index + 1; i < source.size(); ++i) {
			scene->get<Node3D>(source[i])->local_index--;
		}
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

	void Node3DSystem::setDirty(Node3D* node)
	{
		node->active_in_hierarchy_dirty = true;
		for (entity_handle child : node->children)
		{
			setDirty(scene->get<Node3D>(child));
		}
	}

	void Node3DSystem::setParent(Entity entity, Entity parent)
	{
		setParent(entity.getHandle(), parent.getHandle());
	}

	std::vector<entity_handle>& Node3DSystem::getRootNodes()
	{
		return root_nodes;
	}

	void Node3DSystem::onAttachNode(Entity entity)
	{
		Node3D* node = entity.get<Node3D>();
		if (node->parent == NULL_ENTITY_HANDLE)
		{
			root_nodes.push_back(entity.getHandle());
			node->local_index = root_nodes.size() - 1;
			node->depth = 0;
		}
		else
		{
			Node3D* parent_node = scene->get<Node3D>(node->parent);
			parent_node->children.push_back(entity.getHandle());
			node->local_index = parent_node->children.size() - 1;
			node->depth = parent_node->getDepth() + 1;
		}
		if (!entity.has<Transform>())
			node->transform = entity.attach<Transform>();
		else
			node->transform = entity.get<Transform>();

		dirty = true;
		node_count++;
	}

	void Node3DSystem::updateNodeIndices(std::vector<entity_handle> nodes, uint32_t global_index, uint32_t depth)
	{
		uint32_t local_index = 0;
		for (entity_handle node_entity : nodes)
		{
			Node3D* node = scene->get<Node3D>(node_entity);
			updateNodeIndices(node->children, global_index, depth + 1);

			node->local_index = local_index;
			node->global_index = global_index;
			global_index++;
			local_index++;
		}
	}
}
