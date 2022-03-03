#pragma once
#include "Core.h"
#include "vector"

#include "glm/glm.hpp"
#include "string"

namespace HBE {
    struct HB_API MeshData {
        std::vector<uint32_t> indices;
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
		static void load(std::string path, std::vector<std::pair<MeshData, MaterialData>> &meshes);
	};

}