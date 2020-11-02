#include <core/resource/mesh.h>
#include <core/resource/material.h>
#include "core/utility/ModelImporter.h"
class Model {
    std::vector<std::pair<Mesh *, const Material *>> meshes;
public:
    void setMaterial(const Material *material, int mesh_index = 0);

    void load(std::string path);

    const std::vector<std::pair<Mesh *, Material *>>& getMeshes() const;

    ~Model();
};
