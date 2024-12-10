

#include "Model.h"
#include "Resources.h"
#include "map"

#include "GraphicPipeline.h"
#include "Mesh.h"
#include "GraphicPipelineInstance.h"
#include "Texture.h"
#include "core/utility/GLTFLoader.h"

namespace HBE {

	Model::~Model() {
		for (Texture *texture: resources.textures) {
			delete texture;
		}
		for (std::vector<Mesh *> meshes: resources.meshes) {
			for (int i = 0; i < meshes.size(); ++i) {
				delete meshes[i];
			}
		}

		for (GraphicPipelineInstance *material: resources.materials) {
			delete material;
		}
	}

	Model::Model(const ModelInfo &info) {
		this->info = info;
		load(info);
	}


	void Model::load(const ModelInfo &info) {
		this->info = info;
		GLTFLoader loader;
		loader.load(data, info);
		createTextures();
		createMaterials();
		createMeshes();
	}

	const std::vector<ModelNode> &Model::getNodes() {
		return data.nodes;
	}

	const ModelResources &Model::getResources() {
		return resources;
	}

	void Model::createMeshes() {
		for (int mesh_index = 0; mesh_index < data.meshes.size(); ++mesh_index) {
			std::vector<ModelPrimitiveData> primitives = data.meshes[mesh_index].primitives;
			resources.meshes.emplace_back();
			for (int primitive_index = 0; primitive_index < primitives.size(); ++primitive_index) {
				resources.meshes[mesh_index].emplace_back(info.parser->createMesh(primitives[primitive_index], info));
			}
		}
	}

	void Model::createTextures() {
		for (int i = 0; i < data.textures_data.size(); ++i) {
			ModelTextureData &texture_data = data.textures_data[i];
			TextureInfo texture_info{};
			texture_info.width = texture_data.width;
			texture_info.height = texture_data.height;
			texture_info.data = texture_data.data;
			texture_info.format = texture_data.format;
			texture_info.flags = IMAGE_FLAG_NONE;
			texture_info.sampler_info = texture_data.sampler_info;
			resources.textures.emplace_back(Resources::createTexture(texture_info));
		}
	}

	void Model::createMaterials() {
		for (int i = 0; i < data.material_properties.size(); ++i) {
			GraphicPipelineInstance *material = info.parser->createMaterial(data.material_properties[i], resources.textures.data());
			resources.materials.emplace_back(material);
		}
	}


}


