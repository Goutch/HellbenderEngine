#pragma once
#include "Core.h"
#include "core/resource/Mesh.h"
#include "core/utility/Log.h"
#include "core/utility/ModelImporter.h"
#include "core/resource/GraphicPipeline.h"
#include "mutex"
namespace HBE {
    class HB_API Model : public Resource {
        std::string path;
        std::vector<std::pair<Mesh *, GraphicPipeline *>> meshes;
        std::vector<std::pair<MeshData, MaterialData >> meshes_data;
        std::mutex data_mutex;
        void clearMeshes();

    public:
        void setPipeline(GraphicPipeline *pipeline, int mesh_index = 0);

        Model *load(std::string path);

        const std::string &getPath();

        void loadAsync(std::string path);

        void constructModel(std::vector<std::pair<MeshData, MaterialData>> *meshes_data);

        const std::vector<std::pair<Mesh *, GraphicPipeline *>> &getMeshes() const;

        virtual ~Model();

        void constructModel();
    };

}