#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/utility/Log.h>
#include "core/utility/ModelImporter.h"
#include "core/serialization/Serializable.h"
#include "Core.h"

namespace HBE {
    class HB_API Model final : public Resource{
        std::string path;
        std::vector<std::pair<Mesh *, Material *>> meshes;

        void clearMeshes();

    public:
        void setMaterial(Material *material, int mesh_index = 0);

        Model *load(std::string path);

        const std::string &getPath();

        void loadAsync(std::string path);

        void constructModel(std::vector<std::pair<MeshData, MaterialData>> *meshes_data);

        const std::vector<std::pair<Mesh *, Material *>> &getMeshes() const;

        ~Model();

    };

}