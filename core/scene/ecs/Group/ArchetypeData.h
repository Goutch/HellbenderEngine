
#pragma once
namespace HBE {
	template<typename... Components>
	class ArchetypeData {
		void *components[sizeof ...(Components)];

	public:
		void set(uint32_t i, void* data)
		{
			components[i] = data;
		}
		template<std::size_t... I>
		std::tuple<entity_handle, Components&...> createTuple(entity_handle handle, size_t entity_index, std::index_sequence<I...>) {
			return std::forward_as_tuple(
					handle,
					(reinterpret_cast<Components*>(components[I])[entity_index])...
			);
		}
	};
}
