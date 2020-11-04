

#include "Resource.h"
#include "Model.h"


const std::vector<std::pair<Mesh *, Material *>> &Model::getMeshes() const {
    return meshes;
}

Model::~Model() {
    for (int i = 0; i < meshes.size(); ++i) {
        delete meshes[i].first;
        delete meshes[i].second;
    }
}


Model* Model::load(std::string path) {

    Log::status("Loading model:" + path);
    meshes_data = ModelImporter::load(path);
    unsigned int vertex_count=0;

    for (int i = 0; i < meshes_data->size(); ++i) {
        meshes.emplace_back(Mesh::create(), Material::create());
        meshes[i].first->setIndices((*meshes_data)[i].first.indices);
        vertex_count+=(*meshes_data)[i].first.indices.size();
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

        if(!(*meshes_data)[i].second.diffuse_texture_paths.empty())
        {
            auto t=Texture::create();
            t->load((*meshes_data)[i].second.diffuse_texture_paths[0]);
            meshes[i].second->setTexture(t);
        }
    }
    delete meshes_data;
    Log::status(path+" loaded \nVertex count = "+std::to_string(vertex_count));
    return this;
}

void Model::setMaterial(Material *material, int mesh_index) {
    meshes[mesh_index].second = material;
}

Model* Model::create() {
    return new Model();
}
void Model::loadAsync(std::string path) {
  /*  Job<std::string>* job=new Job<std::string>();
    job->setCallback(this,&Model::onFinishLoadingAsync);
    job->run(this,&Model::loadMeshData,path);
    running_jobs.push_back(job);*/
}
void Model::loadMeshData(std::string path) {
    Log::status("Loading asynchronously model:" + path);
    meshes_data = ModelImporter::load(path);
}
void Model::onFinishLoadingAsync() {
    int vertex_count=0;
    for (int i = 0; i < meshes_data->size(); ++i) {
        meshes.emplace_back(Mesh::create(), Material::create());
        meshes[i].first->setIndices((*meshes_data)[i].first.indices);
        vertex_count+=(*meshes_data)[i].first.indices.size();
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

        if(!(*meshes_data)[i].second.diffuse_texture_paths.empty())
        {
            auto t=Texture::create();
            t->load((*meshes_data)[i].second.diffuse_texture_paths[0]);
            meshes[i].second->setTexture(t);
        }
    }
    delete meshes_data;
    Log::status("Loaded model \nVertex count = "+std::to_string(vertex_count));

}





