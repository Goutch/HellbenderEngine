#include "Core.h"
#include "queue"
#include "list"
#include "unordered_map"
#include "unordered_set"
#include "map"
#include "bitset"
#include "typeinfo"
#include "set"
#include "core/utility/Log.h"
#include <cstring>
#include "algorithm"

#ifndef REGISTRY_PAGE_SIZE
#define REGISTRY_PAGE_SIZE 32
#endif
#ifndef REGISTRY_MAX_COMPONENT_TYPES
#define REGISTRY_MAX_COMPONENT_TYPES 128
#endif
namespace HBE {
    typedef uint32_t entity_handle;

    struct ComponentTypeInfo {
        size_t hash;
        uint32_t bit_number;
        size_t size;


        bool operator==(const size_t hash) const {
            return this->hash == hash;
        }

        bool operator==(const ComponentTypeInfo &other) const {
            return hash == other.hash;
        }
    };

    class RawComponentPage {
    public:
        ComponentTypeInfo info;
        char *data;
        bool valid[REGISTRY_PAGE_SIZE];
        std::vector<entity_handle> handles;
        size_t offset;

        RawComponentPage(RawComponentPage &&other) {
            data = other.data;
            memcpy(valid, other.valid, REGISTRY_PAGE_SIZE * sizeof(bool));
            info = other.info;
            offset = other.offset;
        }

        RawComponentPage(ComponentTypeInfo info, size_t offset) {
            data = static_cast<char *>(malloc(info.size));
            memset(data, 0, info.size * REGISTRY_PAGE_SIZE);
            this->offset = offset;
        }

        ~RawComponentPage() {
            delete[] data;
        }

        char *attach(entity_handle handle, void *component) {
            size_t i = handle - offset;
            if (!valid[i]) {
                handles.emplace_back(handle);
                valid[i] = true;
                std::sort(handles.begin(), handles.end());
            }
            memcpy(data + (i * info.size), component, info.size);
            return &data[i];
        }

        template<typename Component>
        Component &getAs(entity_handle handle) {
            return reinterpret_cast<Component *>(data)[handle - offset];
        }

        void detach(entity_handle handle) {
            size_t i = handle - offset;
            if (valid[i]) {
                memset(data + (i * info.size), 0, info.size);
                valid[i] = false;
                handles.erase(std::find(handles.begin(), handles.end(), handle));
                std::sort(handles.begin(), handles.end());
            }
        }

        bool has(entity_handle handle) {
            return valid[handle - offset];
        }
    };

    template<typename Component>
    struct ComponentPage {
        Component *components = nullptr;
        size_t offset = 0;
        std::vector<entity_handle> &handles;

        ComponentPage()
        {

        };

        ComponentPage(RawComponentPage &page) : handles(page.handles) {
            components = reinterpret_cast<Component>(page.data);
            offset = page.offset;
        }

        Component &operator[](size_t i) {
            return components[i];
        }
    };

    class RegistryPage {
    public:
        size_t offset;
        uint32_t count = 0;

        bool valid_entities[REGISTRY_PAGE_SIZE];
        std::bitset<REGISTRY_MAX_COMPONENT_TYPES> components_of_entity[REGISTRY_PAGE_SIZE];
        std::unordered_map<size_t, RawComponentPage *> component_pages;
        std::bitset<REGISTRY_MAX_COMPONENT_TYPES> components_flags = 0;

        RegistryPage(size_t page_index) : offset(page_index * REGISTRY_PAGE_SIZE) {
            for (int i = 0; i < REGISTRY_PAGE_SIZE; ++i) {
                valid_entities[i] = false;
                components_of_entity[i] = 0;
            }
        };

        template<typename Component>
        ComponentPage<Component> getPage() {
            return ComponentPage<Component>(component_pages[typeid(Component).hash_code()]);
        }

        template<typename Component>
        Component &attach(entity_handle handle, ComponentTypeInfo &type) {
            size_t i = handle - offset;
            if (!valid_entities[i]) Log::error("Enitty#" + std::to_string(handle) + "is not valid");


            components_of_entity[i] |= 1 << type.bit_number;
            auto component_page_it = component_pages.find(type.hash);
            if (component_page_it == component_pages.end()) {
                component_pages.emplace(type.hash, new RawComponentPage(type, offset));
                components_flags.set(type.bit_number, true);
            }
            Component component{};
            return *reinterpret_cast<Component *>(component_page_it->second->attach(handle, &component));


        };

        void detach(entity_handle handle, ComponentTypeInfo &type) {
            size_t i = handle - offset;

            components_of_entity[i] |= 1 << type.bit_number;
            if (valid_entities[i]) {
                auto component_page_it = component_pages.find(type.hash);
                if (component_page_it == component_pages.end()) {
                    return;
                }
                RawComponentPage &component_page = *component_page_it->second;
                component_page.detach(handle);
                if (component_page.handles.size() == 0) {
                    components_flags.set(component_page.info.bit_number, false);
                    component_pages.erase(component_page_it->first);
                }
            }
        };

        void setValid(entity_handle handle) {
            valid_entities[handle - offset] = true;
        }

        void setInvalid(entity_handle handle) {
            size_t i = handle - offset;
            valid_entities[i] = false;
            std::list<size_t> obsolete_types_hash;
            for (auto component_page_it: component_pages) {
                RawComponentPage &component_page = *component_page_it.second;
                component_page.detach(i);
                if (component_page.handles.size() == 0) {
                    obsolete_types_hash.emplace_back(component_page_it.first);
                }
            }
            for (size_t hash: obsolete_types_hash) {
                component_pages.erase(hash);
            }
        }

        bool valid(entity_handle handle) {
            return valid_entities[handle - offset];
        }

    };

    template<typename ... Components>
    class Group {

        std::vector<RegistryPage *> &pages;
    public:
        Group(std::vector<RegistryPage *> &pages) : pages(pages) {
            this->pages = pages;
        }

        class iterator {
            std::vector<RegistryPage *> &pages;
            std::vector<entity_handle> entities;
            std::tuple<ComponentPage<Components>...> current_pages;
            size_t current_entity = 0;
            size_t current_page = 0;
            bool finish = false;
        public:
            iterator(std::vector<RegistryPage *> &pages, bool end) : pages(pages),current_pages(std::tuple<ComponentPage<Components>...>(ComponentPage<Components>()...)) {
                this->current_entity = 0;
                finish = end;
                updatePages();
            }

            iterator(iterator &&other) : pages(other.pages) {
                this->current_entity = other.current_entity;
                this->current_page = other.current_page;
                this->components_flags = other.components_flags;
                this->current_pages = other.current_pages;
                this->entities = other.entities;
                this->pages = other.pages;
            }

            iterator(iterator &other) : pages(other.pages) {
                this->current_entity = other.current_entity;
                this->current_page = other.current_page;
                this->components_flags = other.components_flags;
                this->current_pages = other.current_pages;
                this->entities = other.entities;
                this->pages = other.pages;
            }

            template<std::size_t... indices>
            inline std::tuple<entity_handle, Components &...> createTuple(entity_handle handle, std::index_sequence<indices...>) {
                return std::move(std::make_tuple(handle, std::get<indices>(current_pages)[current_entity]...));
            }

            std::tuple<entity_handle, Components &...> operator*() {
                return std::move(createTuple(entities[current_entity], std::index_sequence<sizeof...(Components)>{}));
            }

            bool operator==(const iterator &other) const {
                return current_entity == other.current_entity &&
                       finish == other.finish;
            }

            bool operator!=(const iterator &other) const {
                return current_entity != other.current_entity ||
                       finish != other.finish;
            }

            void updatePages() {
                bool page_changed = false;
                while (current_page != pages.size() &&
                       entities[current_entity] - pages[current_page]->offset > REGISTRY_PAGE_SIZE) {
                    current_page++;
                    page_changed = true;
                }
                if (current_page == pages.size()) {
                    finish = true;
                } else if (page_changed) {
                    current_entity = 0;
                    RawComponentPage* raw_pages[] = {pages[current_page]->component_pages[typeid(Components).hash_code()]...};
                    current_pages = std::move(std::tuple(pages[current_page]->getPage<Components>()...));
                    size_t min_size_index = 0;
                    for (size_t i = 1; i < sizeof...(Components); ++i) {
                        if (raw_pages[min_size_index]->handles.size() > raw_pages[i]->handles.size()) {
                            min_size_index = i;
                        }
                    }
                    entities = raw_pages[min_size_index]->handles;
                    for (size_t i = 0; i < sizeof...(Components); ++i) {
                        if (i == min_size_index)
                            continue;
                        size_t entity_count = entities.size();
                        for (size_t j = 0; j < entity_count; ++j) {
                            while (!std::get<i>(current_pages)->valid(entities[j])) {
                                entity_count--;
                                std::swap(entities[j], entities.back());
                                if (j < entity_count) break;
                            }
                        }
                        entities.resize(entity_count);
                    }
                    std::sort(entities.begin(), entities.end());
                }
            }

            iterator &operator++() {
                current_entity++;
                updatePages();
                return *this;
            }
        };

    public:
        iterator begin() {
            return iterator(pages, false);
        };

        iterator end() {
            return iterator(pages, true);
        }
    };

    class Registry {
        uint32_t current_handle = 0;
        std::queue<entity_handle> inactive;
        std::vector<RegistryPage *> pages;
        std::unordered_map<size_t, ComponentTypeInfo> types;
        uint32_t current_bit = 1;

        inline size_t getPage(entity_handle handle) {
            return (handle - (handle % REGISTRY_PAGE_SIZE)) / REGISTRY_PAGE_SIZE;
        }

    public:
        ~Registry() {
            for (auto page: pages) {
                delete page;
            }
        }

        entity_handle create() {
            entity_handle handle;
            if (inactive.empty()) {
                handle = current_handle;
                current_handle++;
            } else {
                handle = inactive.front();
                inactive.pop();
            }
            size_t page = getPage(handle);
            if (pages.size() < page) {
                pages.resize(page);
            }
            if (pages[handle] == nullptr) {
                pages[page] = new RegistryPage(page);
            }
            pages[page]->setValid(handle);
            return handle;
        }

        template<typename ... Components>
        Group<Components...> group() {
            return Group<Components...>(pages);
        };

        template<typename Component>
        Component &get(entity_handle handle) {
            return pages[getPage(handle)]->component_pages[typeid(Component).hash_code()]->getAs<Component>(handle);
        }

        template<typename Component>
        bool has(entity_handle handle) {
            return pages[getPage(handle)]->valid(handle);
        }

        void destroy(entity_handle handle) {
            inactive.push(handle);
            pages[getPage(handle)]->setInvalid(handle);
        }

        bool valid(entity_handle handle) {
            size_t page = getPage(handle);
            if (pages[page] != nullptr) {
                return pages[page]->valid(handle);
            }
            return false;
        }

        template<typename Component>
        Component &attach(entity_handle handle) {

            const size_t hash = typeid(Component).hash_code();
            auto type_it = types.find(hash);
            if (type_it == types.end()) {
                types.emplace(hash, ComponentTypeInfo{hash, current_bit, sizeof(Component)});
                current_bit++;
            }
            size_t page = getPage(handle);
            return pages[page]->attach<Component>(handle, types[hash]);
        }

        template<typename Component>
        void detach(entity_handle handle) {
            const size_t hash = typeid(Component).hash_code();
            ComponentTypeInfo &type = types[hash];
            size_t page = getPage(handle);

        }

    };
}
