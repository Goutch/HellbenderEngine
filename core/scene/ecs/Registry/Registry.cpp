#include "Registry.h"

namespace HBE {
    std::array<bool, Registry::MAX_REGISTRIES> Registry::free_registry_ids = [] {
        std::array<bool, MAX_REGISTRIES> a{};
        a.fill(true);
        return a;
    }();

    size_t Registry::getPage(entity_handle handle) {
        return (handle - (handle % REGISTRY_PAGE_SIZE)) / REGISTRY_PAGE_SIZE;
    }

    uint32_t Registry::generateTypeIndex(uint64_t component_hash, const char *component_name) {
        auto it = component_hash_to_id.find(component_hash);
        if (it != component_hash_to_id.end()) {
            return it->second;
        }
        uint32_t index = static_cast<uint32_t>(component_hash_to_id.size());
        component_hash_to_id.emplace(component_hash, index);
        Log::debug(
            "registry#" + std::to_string(registry_id) + " Generated new type index : " + std::to_string(index) +
            " for type :" +
            component_name);
        return index;
    }

    uint32_t Registry::findValidRegistryIndex() {
        for (int i = 0; i < free_registry_ids.size(); ++i) {
            if (free_registry_ids[i] == true) {
                free_registry_ids[i] = false;
                return i;
            }
        }
        Log::error("No more registry id available. Increase Registry.MAX_REGISTRY constant");
    }

    void Registry::getAllEntitiesWith(uint32_t type_index, RawVector<entity_handle> &buffer) {
        ComponentTypeInfo component_type_info = types[type_index];
        for (uint32_t i = 0; i < pages.size(); ++i) {
            if (pages[i]->components_signature.test(component_type_info.index)) {
                RawVector<entity_handle> &entities = pages[i]->getRawPool(component_type_info.index)->handles;
                buffer.addRange(entities.data_ptr(), entities.size());
            }
        }
    }

    Registry::Registry() {
        registry_id = findValidRegistryIndex();
    }

    Registry::~Registry() {
        for (auto page: pages) {
            delete page;
        }
        //reset cache
        for (uint32_t *type_cache: component_types_id_caches) {
            *type_cache = INVALID_TYPE_INDEX;
        }
        free_registry_ids[registry_id] = true;
    }

    entity_handle Registry::create() {
        entity_handle handle;
        if (inactive.empty()) {
            handle = current_handle;
            current_handle++;
        } else {
            handle = inactive.front();
            inactive.pop();
        }
        size_t page = getPage(handle);
        if (pages.size() <= page) {
            pages.resize(page + 1, nullptr);
        }
        if (pages[page] == nullptr) {
            pages[page] = new RegistryPage(page);
        }
        pages[page]->setValid(handle);
        return handle;
    }

    std::bitset<128> &Registry::getSignature(entity_handle handle) {
        return pages[getPage(handle)]->getSignature(handle);
    }

    void Registry::destroy(entity_handle handle) {
        inactive.push(handle);

        pages[getPage(handle)]->setInvalid(handle);
    }

    bool Registry::valid(HBE::entity_handle handle) {
        if (handle == NULL_ENTITY_HANDLE) {
            return false;
        }
        size_t page = getPage(handle);
        if (page < pages.size() && pages[page] != nullptr) {
            return pages[page]->valid(handle);
        }
        return false;
    }

    RawVector<RegistryPage *> &Registry::getPages() {
        return pages;
    }

    void Registry::destroyAll() {
        for (auto page: pages) {
            if (page != nullptr) {
                page->destroyAll();
            }
        }
    }
}
