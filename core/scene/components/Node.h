#pragma once

#include <glm/fwd.hpp>

#include "Transform2D.h"
#include "core/scene/Entity.h"
#include "core/scene/ecs/Component.h"
#include "../../../dependencies/utils-collection/data-structure/RawVector.h"

namespace HBE {
	class Transform;

	/**
	 * @brief A node in the scene hierarchy that contains a 3D transform.
	 */

	struct HB_API Node {
		COMPONENT_IDS(Node)

		friend class Scene;

		friend class NodeSystem;
	private:
		uint32_t depth = 0;
		uint32_t local_index;
		uint32_t global_index;
		bool global_index_dirty = true;

		bool is_active = true;
		bool is_active_in_hierarchy = true;
		bool active_in_hierarchy_dirty = true;
	protected:
		entity_handle parent = NULL_ENTITY_HANDLE;
		Scene *scene = nullptr;

	public:
		Entity entity;

		RawVector<entity_handle> children;

		Node();

		explicit Node(Entity entity);

		//Node(Node &&other) noexcept;

		uint32_t getDepth() const;

		uint32_t getLocalIndex() const;

		uint32_t getGlobalIndex() const;

		bool hasParent() const;

		void setParent(Entity entity);

		void setParent(entity_handle entity);

		entity_handle getParentHandle();

		Entity getParent();

		bool isActive() const;

		bool isActiveInHierarchy();


	};
}
