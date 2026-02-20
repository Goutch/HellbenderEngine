//
// Created by username on 3/6/2025.
//

#include "NodeSystem.h"
#include "core/scene/components/Node.h"
#include <core/scene/Scene.h>


namespace HBE {
	NodeSystem::NodeSystem(Scene *scene) : System(scene) {
		scene->onAttach<Node>().subscribe(attach_subscription_id, this, &NodeSystem::onAttachNode);
		scene->onDetach<Node>().subscribe(detach_subscription_id, this, &NodeSystem::onDetachNode);
	}

	NodeSystem::~NodeSystem() {
		scene->onAttach<Node>().unsubscribe(attach_subscription_id);
		scene->onDetach<Node>().unsubscribe(detach_subscription_id);
	}

	void NodeSystem::updateNodeIndices() {
		if (dirty) {
			uint32_t global_index = 0;
			updateNodeIndices(root_nodes, global_index, 0);
		}

	}

	void NodeSystem::onDetachNode(Entity entity) {
		Node *node = entity.get<Node>();

		if (node->hasParent()) {
			Node *parent_node = scene->get<Node>(node->parent);
			parent_node->children.remove(node->getLocalIndex());
		} else {
			for (int i = node->getLocalIndex() + 1; i < root_nodes.size(); ++i) {
				scene->get<Node>(root_nodes[i])->local_index--;
			}
			root_nodes.erase(node->getLocalIndex());
		}

		node_count--;
		dirty = true;
	}

	void NodeSystem::setParent(entity_handle entity, entity_handle parent) {
		Node *node = scene->get<Node>(entity);
		Node *old_parent_node = node->hasParent() ? scene->get<Node>(node->parent) : nullptr;
		Node *new_parent_node = scene->valid(parent) ? scene->get<Node>(parent) : nullptr;
		if (old_parent_node == new_parent_node)
			return;
		RawVector<entity_handle> &source = old_parent_node != nullptr ? old_parent_node->children : root_nodes;
		RawVector<entity_handle> &destination = new_parent_node != nullptr ? new_parent_node->children : root_nodes;

		if (&destination == &source)
			return;

		node->parent = parent;
		for (int i = node->local_index + 1; i < source.size(); ++i) {
			scene->get<Node>(source[i])->local_index--;
		}
		source.erase(node->getLocalIndex());

		size_t size = destination.size();
		destination.emplace_back(entity);

		node->local_index = destination.size() - 1;

		if (scene->has<Transform>(entity)) {
			Transform *transform = scene->get<Transform>(entity);
			transform->setDirty();
		}

		if (scene->has<Transform2D>(entity)) {
			Transform2D *transform = scene->get<Transform2D>(entity);
			transform->setDirty();
		}
		setDirty(node);
		dirty = true;
	}

	void NodeSystem::setDirty(Node *node) {
		node->active_in_hierarchy_dirty = true;
		for (entity_handle child: node->children) {
			setDirty(scene->get<Node>(child));
		}
	}

	void NodeSystem::setParent(Entity entity, Entity parent) {
		setParent(entity.getHandle(), parent.getHandle());
	}

	RawVector<entity_handle> &NodeSystem::getRootNodes() {
		return root_nodes;
	}

	void NodeSystem::onAttachNode(Entity entity) {
		Node *node = entity.get<Node>();
		node->entity = entity;
		if (node->parent == NULL_ENTITY_HANDLE) {
			root_nodes.add(entity.getHandle());
			node->local_index = root_nodes.size() - 1;
			node->depth = 0;
		} else {
			Node *parent_node = scene->get<Node>(node->parent);
			parent_node->children.add(entity.getHandle());
			node->local_index = parent_node->children.size() - 1;
			node->depth = parent_node->getDepth() + 1;
		}

		dirty = true;
		node_count++;
	}

	void NodeSystem::updateNodeIndices(RawVector<entity_handle> &nodes, uint32_t &global_index, uint32_t depth) {
		uint32_t local_index = 0;
		for (entity_handle node_entity: nodes) {
			Node *node = scene->get<Node>(node_entity);
			node->global_index = global_index;
			global_index++;
			updateNodeIndices(node->children, global_index, depth + 1);
			node->local_index = local_index;
			local_index++;
		}
	}

}
