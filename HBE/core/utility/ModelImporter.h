#pragma once
#include "Core.h"
#include "vector"

#include "glm/glm.hpp"
#include "string"

using namespace glm;
namespace HBE {
    struct HB_API MeshData {
        std::vector<unsigned int> indices;
        std::vector<vec3> positions;
        std::vector<vec2> uvs;
        std::vector<vec3> normals;
    };

    struct HB_API MaterialData {
        std::vector<std::string> diffuse_texture_paths;
        std::vector<std::string> specular_map_paths;
        std::vector<std::string> normal_map_paths;
    };

    class HB_API ModelImporter {

    public:
        static std::vector<std::pair<MeshData, MaterialData>> *load(std::string path);
    };

}