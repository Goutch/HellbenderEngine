#pragma once
#include "string"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class Model{
    std::vector<Mesh*> meshes;
public:
    Model(){};
    void load(std::string path);

private:
    void processNode(aiNode *node, const aiScene *scene);

    void processMesh(aiMesh *assimp_mesh, const aiScene *scene);
};
