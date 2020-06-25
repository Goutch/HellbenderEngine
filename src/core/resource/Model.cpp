#include "Model.h"
#include "Texture.h"

#include <core/utility/Log.h>
#include <core/resource/Resource.h>


std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
  /*  std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;*/
    return std::vector<Texture>();
}
void Model::processMesh(aiMesh *assimp_mesh, const aiScene *scene)
{
// data to fill
    std::vector<glm::vec3> vertex_positions;
    std::vector<glm::vec2> vertex_uvs;
    std::vector<glm::vec3> vertex_normal;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // walk through each of the assimp_mesh's vertices
    for(unsigned int i = 0; i < assimp_mesh->mNumVertices; i++)
    {

        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = assimp_mesh->mVertices[i].x;
        vector.y = assimp_mesh->mVertices[i].y;
        vector.z = assimp_mesh->mVertices[i].z;
        vertex_positions.push_back(vector);
        // normals
        vector.x = assimp_mesh->mNormals[i].x;
        vector.y = assimp_mesh->mNormals[i].y;
        vector.z = assimp_mesh->mNormals[i].z;
        vertex_uvs.push_back(vector);
        // texture coordinates
        if(assimp_mesh->mTextureCoords[0]) // does the assimp_mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = assimp_mesh->mTextureCoords[0][i].x;
            vec.y = assimp_mesh->mTextureCoords[0][i].y;
            vertex_uvs.push_back(vec);
        }
        else
        {
            vertex_uvs.push_back( glm::vec2(0.0f, 0.0f));
        }
    }
    // now wak through each of the assimp_mesh's faces (a face is a assimp_mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < assimp_mesh->mNumFaces; i++)
    {
        aiFace face = assimp_mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[assimp_mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
   /* std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
*/

    Mesh* mesh=Resource::get<Mesh>();
    mesh->setIndices(indices);
    mesh->setBuffer(0,vertex_positions);
    mesh->setBuffer(1,vertex_uvs);
    mesh->setBuffer(2,vertex_normal);
    meshes.push_back(mesh);
}
void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}
void Model::load(std::string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Log::error("Failed to load model:"+path+"\nerror:"+ import.GetErrorString());
        return;
    }

    processNode(scene->mRootNode, scene);
}

