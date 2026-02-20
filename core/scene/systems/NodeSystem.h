#pragma once

#include "NodeSystem.h"
#include "core/scene/Entity.h"
#include "core/scene/System.h"
#include "dependencies/utils-collection/RawVector.h"
#include "dependencies/utils-collection/Event.h"
namespace HBE {
	struct Node;

	class NodeSystem : public System {
	private:
		RawVector<entity_handle> root_nodes;
		bool dirty = false;
		uint32_t node_count = 0;
		event_subscription_id attach_subscription_id = 0;
		event_subscription_id detach_subscription_id;
	public:
		NodeSystem(Scene *scene);
		~NodeSystem();
		void updateNodeIndices();

		void onAttachNode(Entity entity);

		void onDetachNode(Entity entity);

		void setParent(entity_handle entity, entity_handle parent);

		void setParent(Entity entity, Entity parent);

		RawVector<entity_handle> &getRootNodes();

		bool isActiveInHierarchy(entity_handle entity);

	private:

		void setDirty(Node *node);

		void updateNodeIndices(RawVector<entity_handle> &nodes, uint32_t& global_index, uint32_t depth);
	};
}
