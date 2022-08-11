

#include <core/threading/JobManager.h>
#include "Model.h"
#include "Resources.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "map"
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
			node.pipeline->setUniform("material", &node.material);
		}
	}


	struct MeshBuffer {
		const void *data = nullptr;
		size_t count = 0;
		size_t element_size = 0;
	};

	//https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_005_BuffersBufferViewsAccessors.md
	//https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes
	Mesh *createPrimitive(tinygltf::Model &model, tinygltf::Primitive &primitive) {
		//mesh
		MeshBuffer indices_buffer{};

		if (primitive.indices != -1) {
			const tinygltf::Accessor index_accessor = model.accessors[primitive.indices];
			const tinygltf::BufferView &index_buffer_view = model.bufferViews[index_accessor.bufferView];
			const tinygltf::Buffer &index_buffer = model.buffers[index_buffer_view.buffer];
			const uint16_t *index_ptr = reinterpret_cast<const uint16_t *>(index_buffer.data.data() + index_buffer_view.byteOffset);
			std::vector<uint16_t> indices = std::vector<uint16_t>(index_ptr, index_ptr + index_accessor.count);
			indices_buffer.data = reinterpret_cast<const void *>(&index_buffer.data.at(index_buffer_view.byteOffset));
			indices_buffer.count = index_accessor.count;
			indices_buffer.element_size = tinygltf::GetComponentSizeInBytes(index_accessor.componentType);
		}

		std::vector<VertexBindingInfo> binding_infos;
		std::vector<MeshBuffer> buffers;
		for (const auto &attribute : primitive.attributes) {
			uint32_t accessor_index = attribute.second;
			std::string attribute_name = attribute.first;

			const tinygltf::Accessor &accessor = model.accessors[accessor_index];
			int buffer_view_index = accessor.bufferView;
			const tinygltf::BufferView &buffer_view = model.bufferViews[buffer_view_index];
			const tinygltf::Buffer &buffer = model.buffers[buffer_view.buffer];
			uint32_t stride = buffer_view.byteStride;
			if (stride == 0) {
				stride = tinygltf::GetNumComponentsInType(accessor.type) * tinygltf::GetComponentSizeInBytes(accessor.componentType);
			}
			//first one add buffer
			if (accessor.byteOffset == 0) {
				VertexBindingInfo binding_info{};
				binding_info.binding = buffers.size();
				binding_info.size = stride;
				binding_info.flags = VERTEX_BINDING_FLAG_NONE;
				binding_infos.emplace_back(binding_info);

				MeshBuffer mesh_buffer{};
				mesh_buffer.data = reinterpret_cast<const void *>(&buffer.data.at(buffer_view.byteOffset));
				mesh_buffer.count = accessor.count;
				mesh_buffer.element_size = stride;
				buffers.emplace_back(mesh_buffer);

				const vec3 *vertices_ptr = reinterpret_cast<const vec3 *>(buffer.data.data() + buffer_view.byteOffset);
				std::vector<vec3> vertices_vector = std::vector<vec3>(vertices_ptr, vertices_ptr + accessor.count);
			}
				//not inside a stride so it is its own buffer
			else if (accessor.byteOffset > stride) {
				VertexBindingInfo binding_info{};
				binding_info.binding = buffers.size();
				binding_info.size = stride;
				binding_info.flags = VERTEX_BINDING_FLAG_NONE;
				binding_infos.emplace_back(binding_info);

				MeshBuffer mesh_buffer{};
				mesh_buffer.data = reinterpret_cast<const void *>(&buffer.data.at(buffer_view.byteOffset));
				mesh_buffer.count = accessor.count;
				mesh_buffer.element_size = stride;
				buffers.emplace_back(mesh_buffer);
			}


		}

		MeshInfo mesh_info{};
		mesh_info.binding_infos = binding_infos.data();
		mesh_info.binding_info_count = binding_infos.size();
		mesh_info.flags = MESH_FLAG_NONE;

		Mesh *mesh_ptr = Resources::createMesh(mesh_info);
		switch (indices_buffer.element_size) {
			case 2:
				mesh_ptr->setVertexIndices(reinterpret_cast<const uint16_t *>(indices_buffer.data), indices_buffer.count);
				break;
			case 4:
				mesh_ptr->setVertexIndices(reinterpret_cast<const uint32_t *>(indices_buffer.data), indices_buffer.count);
				break;
			default:
				break;
		}
		for (size_t i = 0; i < buffers.size(); i++) {
			const MeshBuffer &buffer = buffers[i];
			mesh_ptr->setBuffer(i, buffer.data, buffer.count);
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
			tinygltf::Mesh &mesh = model.meshes[node.mesh];
			for (size_t i = 0; i < mesh.primitives.size(); ++i) {
				tinygltf::Primitive &primitive = mesh.primitives[i];
				model_node.mesh = createPrimitive(model, primitive);
				if (model_node.mesh) {
					model_nodes.push_back(model_node);
				}
				if (primitive.material != -1) {
					//todo:handle material
				}
			}

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

