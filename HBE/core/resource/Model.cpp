

#include "Model.h"
#include "Resources.h"
#include "set"
#include "core/scene/Components.h"
#include <glm/ext/matrix_transform.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "map"
#include "tiny_gltf.h"

#include "GraphicPipeline.h"
#include "Mesh.h"
#include "Material.h"

namespace HBE {

	Model::~Model() {
		for (const std::vector<Mesh *> &primitives:data.meshes) {
			for (Mesh *mesh: primitives) {
				delete mesh;
			}
		}
		for (Material *material:data.materials) {
			delete material;
		}
		for (const ModelMaterialProperties &material_properties:data.material_properties) {
			if (material_properties.texture != nullptr) {
				delete material_properties.texture;
			}
		}
	}

	Model::Model(const ModelInfo &info) {
		load(info);
	}


	struct MeshBuffer {
		const void *data = nullptr;
		size_t count = 0;
		size_t element_size = 0;
	};


	void processNodes(ModelData &data, tinygltf::Model &model, const tinygltf::Node &node, std::vector<ModelNode> &node_array) {
		ModelNode model_node{};

		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			tinygltf::Mesh gltf_mesh = model.meshes[node.mesh];
			for (int i = 0; i < gltf_mesh.primitives.size(); ++i) {
				size_t material_index = gltf_mesh.primitives[i].material;
				model_node.primitives.emplace_back();
				model_node.primitives[i].mesh = data.meshes[node.mesh][i];
				if (data.materials.size() > material_index) {
					model_node.primitives[i].material = data.materials[material_index];
				} else {
					Log::warning("Can't find material for mesh" + std::to_string(i));
				}
			}
		}
		if (node.matrix.size() == 16) {
			for (uint32_t i = 0; i < 4; i++) {
				for (uint32_t j = 0; j < 4; ++j) {
					model_node.transform[j][i] = static_cast<float>(node.matrix[i]);
				}
			}
		} else {
			model_node.transform = mat4(1.0f);
		}
		node_array.emplace_back(model_node);

		for (size_t i = 0; i < node.children.size(); i++) {
			assert((node.children[i] >= 0) && (node.children[i] < (int)model.nodes.size()));
			processNodes(data, model, model.nodes[node.children[i]], model_node.children);
		}
	}

	//https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_005_BuffersBufferViewsAccessors.md
	//https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes
	Mesh *createPrimitive(ModelData &data, const tinygltf::Model &model, const tinygltf::Primitive &primitive, MODEL_FLAGS flags) {
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
		mesh_info.flags |= (flags & MODEL_FLAG_USED_IN_RAYTRACING)==MODEL_FLAG_USED_IN_RAYTRACING ? MESH_FLAG_USED_IN_RAYTRACING : MESH_FLAG_NONE;

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

	void createMeshes(ModelData &data, tinygltf::Model &model, MODEL_FLAGS flags) {
		for (size_t i = 0; i < model.meshes.size(); i++) {
			const tinygltf::Mesh &gltf_mesh = model.meshes[i];
			data.meshes.emplace_back();
			std::vector<Mesh *> &meshes = data.meshes[i];
			for (size_t gltf_primitive_index = 0; gltf_primitive_index < gltf_mesh.primitives.size(); ++gltf_primitive_index) {
				const tinygltf::Primitive &gltf_primitive = gltf_mesh.primitives[gltf_primitive_index];

				Mesh *mesh = createPrimitive(data, model, gltf_primitive, flags);

				meshes.emplace_back(mesh);
			}
		}
	}

	void createMaterials(ModelData &data, tinygltf::Model &model) {
		for (size_t i = 0; i < model.materials.size(); i++) {
			const tinygltf::Material &gltf_material = model.materials[i];
			MaterialInfo material_info{};
			material_info.flags = MATERIAL_FLAG_NONE;
			material_info.graphic_pipeline = Resources::get<GraphicPipeline>("DEFAULT_MODEL_PIPELINE");
			data.materials.emplace_back(Resources::createMaterial(material_info));


			ModelMaterialProperties material_properties{};
			if (gltf_material.pbrMetallicRoughness.baseColorFactor.size() == 4) {
				material_properties.color.r = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor[0]);
				material_properties.color.g = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor[1]);
				material_properties.color.b = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor[2]);
				material_properties.color.a = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor[3]);
			}
			//todo:texture
			data.material_properties.emplace_back(material_properties);

			data.materials[i]->setUniform("material", &data.material_properties[i].color);
		}
	}


	void Model::load(const ModelInfo &info) {
		tinygltf::TinyGLTF loader;
		tinygltf::Model model;
		std::string err;
		std::string warn;
		bool res = loader.LoadASCIIFromFile(&model, &err, &warn, RESOURCE_PATH + info.path);
		if (!warn.empty()) {
			Log::warning(warn);
		}

		if (!err.empty()) {
			Log::error(err);
		}

		if (!res)
			Log::error("Failed to load glTF: ");


		createMeshes(data, model, info.flags);
		if ((info.flags & MODEL_FLAG_DONT_LOAD_MATERIALS) != MODEL_FLAG_DONT_LOAD_MATERIALS) {
			createMaterials(data, model);
		}


		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		//root nodes
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
			processNodes(data, model, model.nodes[scene.nodes[i]], data.nodes);
		}

	}

	const std::vector<ModelNode> &Model::getNodes() {
		return data.nodes;
	}


}

