
#include "ShaderDB.h"
#include "string"
#include "memory"

namespace HBE {

    std::unordered_map<std::string, Shader *> ShaderDB::shaders;

    void ShaderDB::add(const char *name, const std::vector<char> &source, SHADER_TYPE type) {
        if (contains(name)) {
            shaders.erase(name);
        }
        shaders.emplace(name, new Shader(source, type));
    }
    void ShaderDB::add(const char *name, const char* path, SHADER_TYPE type) {
        if (contains(name)) {
            shaders.erase(name);
        }
        Shader* shader=new Shader();
        shader->load(path,type);
        shaders.emplace(name, shader);
    }
    const Shader *ShaderDB::get(const char *name) {
        return shaders.at(name);
    }

    void ShaderDB::remove(const char *name) {
        shaders.erase(name);
    }

    bool ShaderDB::contains(const char *name) {
        return shaders.find(name) != shaders.end();
    }

    void ShaderDB::load(const char* name, const char* path, SHADER_TYPE type) {
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



