#pragma once
namespace HBE {
    template<typename... Components>
    class PageDataArchetype {
        friend class Group<Components...>;
        void *components[sizeof ...(Components)];
        size_t page_offset = 0;

        template<std::size_t... I>
        std::tuple<entity_handle, Components&...> createTuple(entity_handle handle, std::index_sequence<I...>) {
            return std::forward_as_tuple(
                    handle,
                    (reinterpret_cast<Components*>(components[I])[handle-page_offset])...
            );
        }
    public:
        void set(uint32_t i, void *data) {
            components[i] = data;
        }


        std::tuple<entity_handle, Components &...> createTuple(entity_handle handle) {
            return createTuple(handle,std::index_sequence_for<Components...>{});
        }
    };
}
