

#include <core/utility/JobManager.h>
#include "Resource.h"
#include "Model.h"
#include "core/serialization/Serializer.h"

const std::vector<std::pair<Mesh *, Material *>> &Model::getMeshes() const {
    return meshes;
}

Model::~Model() {
    clearModels();
}


Model *Model::load(std::string path) {
    this->path = path;
    Log::status("Loading:" + path);
    auto meshes_data = ModelImporter::load(path);
    constructModel(meshes_data);
    return this;
}

void Model::setMaterial(Material *material, int mesh_index) {
    meshes[mesh_index].second = material;
}

Model *Model::create() {
    return new Model();
}

void Model::loadAsync(std::string path) {
    this->path = path;
    Log::status("Loading async:" + path);
    auto job = JobManager::create<std::vector<std::pair<MeshData, MaterialData>> *, std::string>();
    job->setCallback<Model>(this, &Model::constructModel);
    job->run(&ModelImporter::load, path);

}

void Model::constructModel(std::vector<std::pair<MeshData, MaterialData>> *meshes_data) {
    int vertex_count = 0;
    clearModels();
    for (std::size_t i = 0; i < meshes_data->size(); ++i) {
        meshes.emplace_back(Mesh::create(), Material::create());
        meshes[i].first->setIndices((*meshes_data)[i].first.indices);
        vertex_count += (*meshes_data)[i].first.indices.size();
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

        if (!(*meshes_data)[i].second.diffuse_texture_paths.empty()) {
            auto t = Texture::create();
            t->load((*meshes_data)[i].second.diffuse_texture_paths[0]);
            meshes[i].second->setTexture(t);
        }
    }
    delete meshes_data;
    Log::status(path + " loaded \n\tVertex count = " + std::to_string(vertex_count));

}

void Model::clearModels() {
    for (std::size_t i = 0; i < meshes.size(); ++i) {
        delete meshes[i].first;
        delete meshes[i].second;
    }
}

void Model::serialize(Serializer *serializer) const {
    serializer->begin("Model");
    serializer->addField("path",path);
    serializer->end();
}

const std::string &Model::getPath() {
    return path;
}



