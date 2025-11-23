#pragma once
#include <glm/fwd.hpp>

#include "HBE/core/scene/Entity.h"

namespace HBE
{
    class Transform;

    /**
	 * @brief A node in the scene hierarchy that contains a 3D transform.
	 */
	struct HB_API Node3D
	{
		friend class Scene;
		friend class Node3DSystem;
		Entity entity;
        Transform* transform;
		entity_handle parent = NULL_ENTITY_HANDLE;
		std::vector<entity_handle> children;
		Node3D();
		explicit Node3D(Entity entity);
		Node3D(Node3D&& other) noexcept;

		uint32_t getDepth() const;

		uint32_t getLocalIndex() const;

		uint32_t getGlobalIndex() const;

		bool hasParent() const;

		bool isActive() const;

		bool isActiveInHierarchy();

	private:
		uint32_t depth = 0;
		uint32_t local_index;
		uint32_t global_index;
		bool global_index_dirty = true;

		bool is_active = true;
		bool is_active_in_hierarchy = true;
		bool active_in_hierarchy_dirty = true;
	};

}
