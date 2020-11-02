#include "Model.h"

const std::vector<std::pair<Mesh *, Material *>>& Model::getMeshes() const{
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
    std::vector<std::pair<MeshData,MaterialData>>* meshes_data=ModelImporter::load(path);
    /*for (int i = 0; i < meshes_data; ++i) {
        meshes.emplace(Resource::get<Mesh>());
        meshes[i].first->setIndices(meshes_data[i].indices);
        meshes[i].first->setBuffer(0,meshes_data[i].positions);
        meshes[i].first->setBuffer(1,meshes_data[i].uvs);
        meshes[i].first->setBuffer(2,meshes_data[i].normals);
    }*/
    delete meshes_data;
}

void Model::setMaterial(const Material *material, int mesh_index) {
    meshes[mesh_index].second=material;
}
