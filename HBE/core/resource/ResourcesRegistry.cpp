#include "ResourcesRegistry.h"

namespace HBE {
    std::unordered_map<std::string, Resource *> ResourcesRegistry::registry;

    void ResourcesRegistry::clear() {
        for (auto &name_res:ResourcesRegistry::registry) {
            delete name_res.second;
        }
        ResourcesRegistry::registry.clear();
    }

    bool ResourcesRegistry::exist(std::string &name) {
        return ResourcesRegistry::registry.find(name) != registry.end();
    }

    void ResourcesRegistry::add(std::string &name, Resource *res) {
        ResourcesRegistry::registry.emplace(name, res);
    }

    Resource *ResourcesRegistry::get(std::string &name) {
#ifdef DEBUG_MODE
        if (!exist(name)) {
            Log::error("Cant find resource:" + name);
        }
#endif
        return ResourcesRegistry::registry.at(name);
    }

    void ResourcesRegistry::remove(std::string &name) {
        delete get(name);
        ResourcesRegistry::registry.erase(name);
    }
}