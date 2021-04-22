
#include "ShaderDB.h"
#include "string"
#include "memory"

namespace HBE {

    std::unordered_map<std::string, Shader *> ShaderDB::shaders;

    void ShaderDB::add(const std::string &name, const std::string &source, SHADER_TYPE type) {
        if (contains(name)) {
            shaders.erase(name);
        }
        shaders.emplace(name, new Shader(source, type));
    }

    const Shader *ShaderDB::get(const std::string &name) {
        return shaders.at(name);
    }

    void ShaderDB::remove(const std::string &name) {
        shaders.erase(name);
    }

    bool ShaderDB::contains(const std::string &name) {
        return shaders.find(name) != shaders.end();
    }

    void ShaderDB::load(const std::string &name, std::string &path, SHADER_TYPE type) {
        if (contains(name)) {
            shaders.erase(name);
        }
        shaders.emplace(name, new Shader());
        shaders.at(name)->load(path, type);

    }

    void ShaderDB::terminate() {
        for (auto s:shaders) {
            delete s.second;
        }
        shaders.clear();
    }


}



