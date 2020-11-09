#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/utility/Log.h>
#include "core/utility/ModelImporter.h"
#include "Resource.h"
#include "Core.h"
class HB_API Model :Resource{
    std::string path;
    std::vector<std::pair<Mesh *,Material *>> meshes;
    void clearModels();
public:
    static Model* create();
    void setMaterial(Material *material, int mesh_index = 0);

    Model * load(std::string path);
    void loadAsync(std::string path);
    void constructModel(std::vector<std::pair<MeshData, MaterialData>> * meshes_data);
    const std::vector<std::pair<Mesh *,Material *>>& getMeshes() const;

    ~Model();

    void serialize(Serializer* serializer) const override;
};

