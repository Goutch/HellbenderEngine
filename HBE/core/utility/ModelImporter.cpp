#include "ModelImporter.h"
#include "Log.h"
#include "fstream"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::vector<std::string> loadMaterialTextures(aiMaterial *mat, aiTextureType type) {
    std::vector<std::string> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString path;
        mat->GetTexture(type, i, &path);
        textures.emplace_back(path.C_Str());
    }
    return textures;
}

std::pair<MeshData, MaterialData> processMesh(aiMesh *mesh, const aiScene *scene) {

    MeshData mesh_data;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        mesh_data.positions.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if(i<mesh_data.normals.size())
            mesh_data.normals.emplace_back(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0])mesh_data.uvs.emplace_back(vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
            mesh_data.indices.emplace_back();
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            mesh_data.indices.emplace_back(face.mIndices[j]);
    }

    MaterialData material_data;

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        material_data.diffuse_texture_paths = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        material_data.specular_map_paths = loadMaterialTextures(material, aiTextureType_SPECULAR);
        material_data.normal_map_paths = loadMaterialTextures(material, aiTextureType_NORMALS);
    }
    return std::make_pair(mesh_data, material_data);
}

void processNode(aiNode *node, const aiScene *scene, std::vector<std::pair<MeshData, MaterialData>> *meshes) {
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes->push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, meshes);
    }
}

std::vector<std::pair<MeshData, MaterialData>> *ModelImporter::load(std::string path) {
    auto meshes = new std::vector<std::pair<MeshData, MaterialData>>();

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Log::error(std::string("ASSIMP ERROR:") + import.GetErrorString());
    }
    std::string directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene, meshes);

    return meshes;
}

