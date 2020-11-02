#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/utility/Log.h>
#include "core/utility/ModelImporter.h"
class Model {
    std::vector<std::pair<Mesh *,Material *>> meshes;
public:
    void setMaterial(Material *material, int mesh_index = 0);

    void load(std::string path);

    const std::vector<std::pair<Mesh *,Material *>>& getMeshes() const;

    ~Model();
};

