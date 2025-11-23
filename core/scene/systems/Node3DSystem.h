#pragma once

#include "core/scene/Entity.h"
#include "core/scene/System.h"
#include "core/scene/ecs/ComponentPool.h"

namespace HBE
{
	struct  Node3D;

	class Node3DSystem : public System
	{
	private:
		std::vector<entity_handle> root_nodes;
		bool dirty = false;
		uint32_t node_count = 0;

	public:
		Node3DSystem(Scene* scene);

		void updateNodeIndices();
		void onAttachNode(Entity entity);
		void onDetachNode(Entity entity);
		void setParent(entity_handle entity, entity_handle parent);
		void setDirty(Node3D* node);
		void setParent(Entity entity, Entity parent);
		std::vector<entity_handle>& getRootNodes();
		bool isActiveInHierarchy(entity_handle entity);
	private:
		void updateNodeIndices(std::vector<entity_handle> nodes, uint32_t global_index, uint32_t depth);
	};
}
