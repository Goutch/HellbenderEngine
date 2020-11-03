#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/utility/Log.h>
#include "core/utility/ModelImporter.h"
#include "Resource.h"
class Model :Resource<Model>{
    std::vector<std::pair<Mesh *,Material *>> meshes;
public:
    static Model* create();
    void setMaterial(Material *material, int mesh_index = 0);

    Model * load(std::string path);

    const std::vector<std::pair<Mesh *,Material *>>& getMeshes() const;

    ~Model();
};

