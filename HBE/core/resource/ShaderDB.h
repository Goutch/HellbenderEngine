#include "Core.h"
#include "string"
#include "memory"
#include "unordered_map"
#include "Shader.h"

namespace HBE {
    class HB_API ShaderDB {
        static std::unordered_map<std::string, Shader*> shaders;
    public:
        static void add(const std::string &name,const std::string &source, SHADER_TYPE type);

        static void load(const std::string &name, std::string &path, SHADER_TYPE type);

        static const Shader *get(const std::string &name);

        static void remove(const std::string &name);

        static bool contains(const std::string &name);

        static void terminate();
    };

}
