#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/utility/Log.h>
#include "core/utility/ModelImporter.h"
#include "Resource.h"
class Model :Resource{
    std::vector<std::pair<Mesh *,Material *>> meshes;
    std::vector<std::pair<MeshData, MaterialData>> *meshes_data;
public:
    static Model* create();
    void setMaterial(Material *material, int mesh_index = 0);

    Model * load(std::string path);
    void loadAsync(std::string path);
    void loadMeshData(std::string path);
    void onFinishLoadingAsync();
    const std::vector<std::pair<Mesh *,Material *>>& getMeshes() const;

    ~Model();
};

