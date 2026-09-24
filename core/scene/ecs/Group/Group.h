#pragma once

#include <thread>

#include "Core.h"
#include "core/scene/ecs/Registry/Registry.h"
#include "GroupIterator.h"
#include "../../../../dependencies/utils-collection/data-structure/RawVector.h"
#include "core/scene/ecs/Group/PageDataArchetype.h"

namespace HBE {
    template<typename... Components>
    class GroupIterator;

    enum GROUP_THREAD_ALLOCATION_TYPE {
        GROUP_THREAD_ALLOCATION_TYPE_PER_ENTITY,
        GROUP_THREAD_ALLOCATION_TYPE_PER_PAGE, //one thread per REGISTRY_PAGE_SIZE entities
    };

    template<typename... Components>
    class Group {
        Registry *registry;
        RawVector<PageDataArchetype<Components...> > pages_data;
        //Each PageDataArchetype contain a void* of each Components types
        RawVector<entity_handle> *page_entity_handles; // Each vector is the entities in a page
        size_t page_count = 0;

    public:
        Group(Registry *registry) : registry(registry) {
            bool empty = true;
            ComponentTypeInfo types[sizeof ...(Components)];
            registry->getComponentsTypeInfo<Components...>(types);

            RawVector<RegistryPage *> &pages = registry->getPages();
            pages_data.reserve(pages.size());

            std::bitset<REGISTRY_MAX_COMPONENT_TYPES> required_signature = 0;
            for (uint32_t i = 0; i < sizeof...(Components); ++i) {
                required_signature.set(types[i].index, true);
            }
            for (uint32_t i = 0; i < pages.size(); ++i) {
                PageDataArchetype<Components...> archetype_data{};

                for (int j = 0; j < sizeof...(Components); ++j) {
                    RawComponentPool *pool = pages[i]->getRawPool(types[j].index);
                    if (pool != nullptr) {
                        archetype_data.set(j, reinterpret_cast<void *>(pool->data));
                    } else {
                        archetype_data.set(j, nullptr);
                    }
                }
                archetype_data.page_offset = i * REGISTRY_PAGE_SIZE;
                pages_data.add(archetype_data);
            }

            page_entity_handles = new RawVector<entity_handle>[pages.size()];
            page_count = pages.size();

            for (uint32_t i = 0; i < pages.size(); ++i) {
                RegistryPage *page = pages[i];

                //skip if there is no matching components in this page
                if ((page->components_signature & required_signature) != required_signature)
                    continue;


                //find the pool with the least entities
                RawComponentPool *min_size_pool = nullptr;
                uint32_t min_size = UINT32_MAX;
                for (uint32_t j = 0; j < sizeof ...(Components); ++j) {
                    RawComponentPool *pool = pages[i]->getRawPool(types[j].index);
                    if (pool->handles.size() < min_size) {
                        min_size_pool = pool;
                        min_size = min_size_pool->handles.size();
                    }
                }
                //create the list of entity indices for this page
                page_entity_handles[i].reserve(min_size_pool->handles.size());
                RawVector<entity_handle> &handles = min_size_pool->handles;

                for (int j = 0; j < handles.size(); ++j) {
                    entity_handle handle = handles[j];
                    std::bitset<REGISTRY_MAX_COMPONENT_TYPES> entity_signature = page->getSignature(handle);
                    if ((entity_signature & required_signature) == required_signature) {
                        page_entity_handles[i].add(handle);
                    }
                }
                if (!page_entity_handles[i].empty()) {
                    empty = false;
                }
            }

            if (empty) {
                page_count = 0;
            }
        }

        ~Group() {
            delete[] page_entity_handles;
        };

        template<typename Func>
        void forEach(Func &&func) {
            for (auto it = begin(); it != end(); ++it) {
                std::apply(
                    [&](entity_handle e, Components &... comps) {
                        func(e, comps...);
                    },
                    *it
                );
            }
        }

        /**
         * Each entity is processes individually by threads, used when entity count is low.
         */
        template<typename Func>
        void forEachEntityParallel(Func &&func, uint32_t thread_count = std::thread::hardware_concurrency()) {
            struct AtomicIteratorState {
                uint32_t page_index;
                uint32_t entity_index;

                uint64_t pack() const {
                    return (static_cast<uint64_t>(page_index) << 32) | entity_index;
                }

                static AtomicIteratorState unpack(uint64_t v) {
                    return {static_cast<uint32_t>(v >> 32), static_cast<uint32_t>(v & 0xFFFFFFFF)};
                }
            };
            std::atomic<uint64_t> iteration_state{0};
            auto entity_worker = [&]() {
                while (true) {
                    uint64_t old_raw = iteration_state.load(std::memory_order_relaxed);
                    uint64_t new_raw = old_raw;
                    AtomicIteratorState new_state;
                    AtomicIteratorState old_state;
                    do {
                        old_state = AtomicIteratorState::unpack(old_raw);
                        new_state = old_state;

                        new_state.entity_index++;

                        while (new_state.entity_index >= page_entity_handles[new_state.page_index].size()) {
                            new_state.entity_index = 0;
                            new_state.page_index++;
                            if (new_state.page_index >= page_count) {
                                return;
                            }
                        }
                        old_raw = old_state.pack();
                        new_raw = new_state.pack();
                        //try to set state to new if it was never changed,
                        // on success set the state
                        // of fail set the old
                    } while (!iteration_state.compare_exchange_weak(old_raw,
                                                                    new_raw,
                                                                    std::memory_order_acquire,
                                                                    std::memory_order_relaxed));

                    entity_handle current_handle = page_entity_handles[new_state.page_index][new_state.
                        entity_index];
                    pages_data[new_state.page_index].call(current_handle, func);
                }
            };

            std::vector<std::thread> threads;
            threads.reserve(thread_count);

            for (size_t i = 0; i < thread_count; ++i)
                threads.emplace_back(entity_worker);

            for (auto &t: threads)
                t.join();
        }

        /**
         * Each thread process (REGISTRY_PAGE_SIZE) entities. Good for system that process thread_count*REGISTRY_PAGE_SIZE or more.
         */
        template<typename Func>
        void forEachPageParallel(Func &&func, uint32_t thread_count = std::thread::hardware_concurrency()) {
            std::atomic<size_t> page_index{0};

            auto page_worker = [&]() {
                while (true) {
                    size_t assigned_page_index = page_index.fetch_add(1, std::memory_order_relaxed);

                    if (assigned_page_index >= page_count)
                        break;

                    RawVector<entity_handle> &page_entities = page_entity_handles[assigned_page_index];
                    if (page_entities.size() == 0)
                        continue;

                    PageDataArchetype<Components...> &archetype_data = pages_data[assigned_page_index];

                    for (uint32_t i = 0; i < page_entities.size(); ++i) {
                        archetype_data.call(page_entities[i], func);
                    }
                }
            };

            std::vector<std::thread> threads;
            threads.reserve(thread_count);

            for (size_t i = 0; i < thread_count; ++i)
                threads.emplace_back(page_worker);

            for (auto &t: threads)
                t.join();
        }


        GroupIterator<Components...> begin() {
            return GroupIterator<Components...>(pages_data, page_entity_handles, page_count);
        };

        GroupIterator<Components...> end() {
            return GroupIterator<Components...>(pages_data, page_entity_handles, page_count, page_count,
                                                static_cast<size_t>(0));
        }
    };
}
