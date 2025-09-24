#pragma once
#include <glm/fwd.hpp>

#include "core/scene/Entity.h"

namespace HBE
{
	struct HB_API HierarchyNode
	{
		friend class Scene;
		friend class HierarchySystem;
		Entity entity;
		entity_handle parent = NULL_ENTITY_HANDLE;
		std::vector<entity_handle> children;
		HierarchyNode();
		explicit HierarchyNode(Entity entity);
		HierarchyNode(HierarchyNode&& other) noexcept;

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
