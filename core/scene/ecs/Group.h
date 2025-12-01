#pragma once

#include "Core.h"
#include "RegistryPage.h"
#include "ComponentPool.h"
#include "GroupIterator.h"
#include "SubGroup.h"

namespace HBE
{
	template <typename... Components>
	class SubGroup;
	template <typename... Components>
	class GroupIterator;

	template <typename... Components>
	class Group
	{
		std::vector<RegistryPage*>& pages;
		ComponentTypeInfo types[sizeof...(Components)];
		bool empty = false;

	public:
		Group(std::vector<RegistryPage*>& pages) : pages(pages)
		{
			empty = true;
		}

		Group(std::vector<RegistryPage*>& pages, ComponentTypeInfo types[sizeof...(Components)]) : pages(pages)
		{
			for (size_t i = 0; i < sizeof...(Components); i++)
			{
				this->types[i] = types[i];
			}
			empty = pages.size() == 0;
		}

		std::vector<SubGroup<Components...>> split()
		{
			std::vector<SubGroup<Components...>> sub_groups;
			for (auto page : pages)
			{
				sub_groups.push_back(SubGroup<Components...>(page, types));
			}
			return sub_groups;
		}

		GroupIterator<Components...> begin()
		{
			if (empty)return GroupIterator<Components...>(pages);
			else return GroupIterator<Components...>(pages, types, false);
		};

		GroupIterator<Components...> end()
		{
			if (empty)return GroupIterator<Components...>(pages);
			else return GroupIterator<Components...>(pages, types, true);
		}
	};
}
