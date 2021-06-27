#include "Core.h"
#include "string"
#include "memory"
#include "unordered_map"
#include "Shader.h"

namespace HBE {
    class HB_API ShaderDB {
        static std::unordered_map<std::string, Shader *> shaders;
    public:
        static void add(const char *name, const std::vector<char> &source, SHADER_TYPE type);

        static void add(const char *name, const char *source, SHADER_TYPE type);

        static void load(const char *name, const char *path, SHADER_TYPE type);

        static const Shader *get(const char *name);

        static void remove(const char *name);

        static bool contains(const char *name);

        static void terminate();


    };

}
