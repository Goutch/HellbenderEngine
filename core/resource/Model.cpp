#include "Model.h"
#include "Resources.h"
#include "map"

#include "GraphicPipeline.h"
#include "Mesh.h"
#include "GraphicPipelineInstance.h"
#include "Texture.h"
#include "core/utility/GLTFLoader.h"
#include "core/resource/raytracing/AccelerationStructure.h"

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

		for (MeshAccelerationStructure *acceleration_structure: resources.acceleration_structures) {
			delete acceleration_structure;
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
		info.parser->onStartParsingModel(this);
		createTextures();
		createMaterials();
		createMeshes();
		if (info.flags & MODEL_FLAG_USED_IN_RAYTRACING) {
			createAccelerationStructures();
			createAccelerationStructureInstances();
		}
		info.parser->onEndParsingModel(this);
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
			resources.textures.emplace_back(info.parser->createTexture(data.textures_data[i]));
		}
	}

	void Model::createMaterials() {
		for (int i = 0; i < data.material_properties.size(); ++i) {
			GraphicPipelineInstance *material = info.parser->createMaterial(data.material_properties[i], resources.textures.data());
			resources.materials.emplace_back(material);
		}
	}

	void Model::createAccelerationStructures() {
		for (int i = 0; i < resources.meshes.size(); ++i) {
			for (int j = 0; j < resources.meshes[i].size(); ++j) {
				resources.acceleration_structures.emplace_back(info.parser->createMeshAccelerationStructure(resources.meshes[i][j], i));
			}
		}
	}

	const std::vector<AccelerationStructureInstance> &Model::getAccelerationStructureInstances() {
		return resources.acceleration_structure_instances;
	}

	void Model::createAccelerationStructureInstances() {
		for (int i = 0; i < data.nodes.size(); ++i) {
			const ModelNode &node = data.nodes[i];
			for (int j = 0; j < node.primitives.size(); ++j) {
				resources.acceleration_structure_instances.emplace_back(info.parser->createAccelerationStructureInstance(data.nodes[i], j));
			}
		}
	}


}
