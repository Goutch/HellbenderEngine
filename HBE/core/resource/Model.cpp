

#include <core/threading/JobManager.h>
#include "Model.h"
#include "Resources.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "tiny_gltf.h"

namespace HBE {
	Model::~Model() {
		for (auto &node:nodes) {
			delete node.mesh;
		}
		nodes.clear();
	}

	Model::Model(const ModelInfo &info) {
		load(info.path);
		for (auto &node:nodes) {
			node.pipeline = Resources::get<GraphicPipeline>("MODEL_PIPELINE");
		}
	}


	Mesh *bindMesh(tinygltf::Model &model, tinygltf::Mesh &mesh) {
		//mesh
		VertexBindingInfo binding_info{};
		binding_info.binding = 0;
		binding_info.size = sizeof(vec3)+sizeof(vec3);
		binding_info.flags = VERTEX_BINDING_FLAG_NONE;

		MeshInfo mesh_info{};
		mesh_info.binding_infos = &binding_info;
		mesh_info.binding_info_count = 1;
		mesh_info.flags = MESH_FLAG_NONE;


		Mesh *mesh_ptr = Resources::createMesh(mesh_info);
		for (size_t i = 0; i < mesh.primitives.size(); ++i) {
			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor index_accessor = model.accessors[primitive.indices];
			const tinygltf::BufferView &index_buffer_view = model.bufferViews[index_accessor.bufferView];
			if (index_buffer_view.target != 0) {
				const tinygltf::Buffer &index_buffer = model.buffers[index_buffer_view.buffer];
				size_t index_count = index_buffer_view.byteLength / sizeof(uint32_t);
				std::vector<uint32_t> indices;
				indices.assign(index_buffer.data.begin(), index_buffer.data.begin() + index_buffer_view.byteLength);
				indices.resize(index_count);
				mesh_ptr->setVertexIndices(indices);
			}
			for (auto &attrib : primitive.attributes) {
				tinygltf::Accessor accessor = model.accessors[attrib.second];
				const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
				int vaa = -1;
				if (attrib.first.compare("POSITION") == 0) vaa = 0;
					else if (attrib.first.compare("NORMAL") == 0) vaa = 1;
					//else if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
				else continue;

				if (bufferView.target == 0) {
					continue;
				}
				const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

				size_t count = bufferView.byteLength / binding_info.size;

				mesh_ptr->setBuffer(0, &buffer.data.at(bufferView.byteOffset), count);
			}
		}


		return mesh_ptr;
	}

	void bindModelNodes(tinygltf::Model &model, const tinygltf::Node &node, std::vector<ModelNode> &model_nodes) {
		ModelMaterial material{};
		material.color = vec4(1.0f);
		material.texture = nullptr;

		ModelNode model_node{};
		model_node.material = material;

		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {

			model_node.mesh = bindMesh(model, model.meshes[node.mesh]);
			if (model_node.mesh)
				model_nodes.push_back(model_node);
		}

		for (size_t i = 0; i < node.children.size(); i++) {
			assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
			bindModelNodes(model, model.nodes[node.children[i]], model_nodes);
		}
	}

	void Model::load(const std::string &path) {
		tinygltf::TinyGLTF loader;
		tinygltf::Model model;
		std::string err;
		std::string warn;
		bool res = loader.LoadASCIIFromFile(&model, &err, &warn, RESOURCE_PATH + path);
		if (!warn.empty()) {
			Log::warning(warn);
		}

		if (!err.empty()) {
			Log::error(err);
		}

		if (!res)
			Log::error("Failed to load glTF: ");


		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		//root nodes
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
			bindModelNodes(model, model.nodes[scene.nodes[i]], nodes);
		}

	}

	std::vector<ModelNode> Model::getNodes() {
		return nodes;
	}


}

