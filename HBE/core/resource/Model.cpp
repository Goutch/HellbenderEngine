

#include "Model.h"
#include "Resources.h"
#include "set"
#include "core/scene/Components.h"
#include <glm/ext/matrix_transform.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "map"
#include "tiny_gltf.h"

namespace HBE {
	Model::~Model() {
		for (std::vector<Mesh *> primitives:data.meshes) {
			for (Mesh *mesh: primitives) {
				delete mesh;
			}
		}
		for (ModelMaterial material:data.materials) {
			delete material.texture;
		}
	}

	Model::Model(const ModelInfo &info) {
		load(info.path);
	}


	struct MeshBuffer {
		const void *data = nullptr;
		size_t count = 0;
		size_t element_size = 0;
	};


	void Model::processNodes(tinygltf::Model &model, const tinygltf::Node &node, std::vector<ModelNode> &node_array) {
		ModelNode model_node{};


		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			tinygltf::Mesh gltf_mesh = model.meshes[node.mesh];
			for (int i = 0; i < gltf_mesh.primitives.size(); ++i) {
				size_t material_index = gltf_mesh.primitives[i].material;
				model_node.primitives.emplace_back();
				model_node.primitives[i].mesh = data.meshes[node.mesh][i];
				if (material_index != -1)
					model_node.primitives[i].material = data.materials[material_index];
				else
					model_node.primitives[i].material = ModelMaterial{};
				model_node.primitives[i].material.pipeline = Resources::get<GraphicPipeline>("DEFAULT_MODEL_PIPELINE");
			}
		}
		if (node.matrix.size() == 16) {
			for (size_t i = 0; i < 4; i++) {
				for (int j = 0; j < 4; ++j) {
					model_node.transform[i][j] = static_cast<float>(node.matrix[i]);
				}
			}
		} else {
			model_node.transform = mat4(1.0f);
		}
		node_array.emplace_back(model_node);

		for (size_t i = 0; i < node.children.size(); i++) {
			assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
			processNodes(model, model.nodes[node.children[i]], model_node.children);
		}
	}

	//https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_005_BuffersBufferViewsAccessors.md
	//https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes
	Mesh *createPrimitive(const tinygltf::Model &model, const tinygltf::Primitive &primitive) {
		//mesh
		MeshBuffer indices_buffer{};

		if (primitive.indices != -1) {
			const tinygltf::Accessor index_accessor = model.accessors[primitive.indices];
			const tinygltf::BufferView &index_buffer_view = model.bufferViews[index_accessor.bufferView];
			const tinygltf::Buffer &index_buffer = model.buffers[index_buffer_view.buffer];
			indices_buffer.data = reinterpret_cast<const void *>(&index_buffer.data.at(index_buffer_view.byteOffset));
			indices_buffer.count = index_accessor.count;
			indices_buffer.element_size = tinygltf::GetComponentSizeInBytes(index_accessor.componentType);
		}

		std::vector<VertexBindingInfo> binding_infos;
		std::vector<MeshBuffer> buffers;

		//Interleaved if buffer view is equal and accessor offset is less than stride
		//non Interleaved if accessor offset is == stride
		std::set<uint32_t> accessors;
		for (const auto &attribute : primitive.attributes) {
			std::string attribute_name = attribute.first;
			if (attribute_name == "TANGENT") {
				continue;
			}
			accessors.emplace(attribute.second);
		}
		for (uint32_t accessor_index : accessors) {
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

	void Model::createMeshes(tinygltf::Model &model) {
		for (size_t i = 0; i < model.meshes.size(); i++) {
			const tinygltf::Mesh &gltf_mesh = model.meshes[i];
			data.meshes.emplace_back();
			std::vector<Mesh *> &meshes = data.meshes[i];
			for (size_t gltf_primitive_index = 0; gltf_primitive_index < gltf_mesh.primitives.size(); ++gltf_primitive_index) {
				const tinygltf::Primitive &gltf_primitive = gltf_mesh.primitives[gltf_primitive_index];

				Mesh *mesh = createPrimitive(model, gltf_primitive);

				meshes.emplace_back(mesh);
			}
		}
	}

	void Model::createMaterials(tinygltf::Model &model) {
		for (size_t i = 0; i < model.materials.size(); i++) {
			const tinygltf::Material &gltf_material = model.materials[i];
			data.materials.emplace_back();
			ModelMaterial &material = data.materials[i];
			if (gltf_material.pbrMetallicRoughness.baseColorFactor.size() == 4) {
				material.color.r = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor[0]);
				material.color.g = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor[1]);
				material.color.b = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor[2]);
				material.color.a = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor[3]);
			}
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


		createMeshes(model);
		createMaterials(model);


		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		//root nodes
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
			processNodes(model, model.nodes[scene.nodes[i]], data.nodes);
		}

	}

	const std::vector<ModelNode>& Model::getNodes() {
		return data.nodes;
	}


}

