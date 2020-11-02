
#include "Resource.h"

const std::vector<std::pair<Mesh *, Material *>> &Model::getMeshes() const {
    return meshes;
}

Model::~Model() {
    for (int i = 0; i < meshes.size(); ++i) {
        delete meshes[i].first;
        delete meshes[i].second;
    }
}

void Model::load(std::string path) {
    Log::status("Loading model:" + path);
    std::vector<std::pair<MeshData, MaterialData>> *meshes_data = ModelImporter::load(path);
    for (int i = 0; i < meshes_data->size(); ++i) {
        meshes.emplace_back(Resource::create<Mesh>(), Resource::create<Material>());
        meshes[i].first->setIndices((*meshes_data)[i].first.indices);
        if (!(*meshes_data)[i].first.positions.empty()) {
            meshes[i].first->setBuffer(0, (*meshes_data)[i].first.positions);
        }
        if (!(*meshes_data)[i].first.uvs.empty()) {
            meshes[i].first->setBuffer(1, (*meshes_data)[i].first.uvs);
        }
        if (!(*meshes_data)[i].first.normals.empty()) {
            meshes[i].first->setBuffer(2, (*meshes_data)[i].first.normals);
        }


        meshes[i].second->setShader(Graphics::DEFAULT_MESH_SHADER);
        meshes[i].second->setColor(vec4(1, 0, 0, 1));
    }
    delete meshes_data;
}

void Model::setMaterial(Material *material, int mesh_index) {
    meshes[mesh_index].second = material;
}