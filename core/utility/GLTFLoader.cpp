#include <set>
#include "GLTFLoader.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "tiny_gltf.h"
#include "core/resource/Model.h"
#include "core/resource/Texture.h"

namespace HBE
{
	void processNodes(ModelData& data, tinygltf::Model& model, const tinygltf::Node& node,
	                  std::vector<ModelNode>& node_array)
	{
		ModelNode model_node{};
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
		{
			model_node.mesh = node.mesh;
			tinygltf::Mesh gltf_mesh = model.meshes[node.mesh];
			for (int i = 0; i < gltf_mesh.primitives.size(); ++i)
			{
				size_t material_index = gltf_mesh.primitives[i].material;
				model_node.primitives.emplace_back();
				HB_ASSERT(material_index >= 0 && material_index < model.materials.size(),
				          "Material index out of bounds");
				model_node.primitives[i].material = material_index;
			}
		}
		if (node.matrix.size() == 16)
		{
			for (uint32_t i = 0; i < 4; i++)
			{
				for (uint32_t j = 0; j < 4; ++j)
				{
					model_node.transform[j][i] = static_cast<float>(node.matrix[i]);
				}
			}
		}
		else
		{
			model_node.transform = mat4(1.0f);
		}
		node_array.emplace_back(model_node);

		for (size_t i = 0; i < node.children.size(); i++)
		{
			assert((node.children[i] >= 0) && (node.children[i] < (int) model.nodes.size()));
			processNodes(data, model, model.nodes[node.children[i]], model_node.children);
		}
	}

	//https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_005_BuffersBufferViewsAccessors.md
	//https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes
	void createMeshData(ModelData& data, tinygltf::Model& model, MODEL_FLAGS flags)
	{
		for (size_t mesh_index = 0; mesh_index < model.meshes.size(); mesh_index++)
		{
			const tinygltf::Mesh& gltf_mesh = model.meshes[mesh_index];
			data.meshes.emplace_back();
			for (size_t primitive_index = 0; primitive_index < gltf_mesh.primitives.size(); ++primitive_index)
			{
				ModelMeshData& mesh_data = data.meshes[mesh_index];
				mesh_data.primitives.emplace_back();
				const tinygltf::Primitive& gltf_primitive = gltf_mesh.primitives[primitive_index];
				ModelPrimitiveData& primitive = data.meshes[mesh_index].primitives.back();
				if (TINYGLTF_MODE_TRIANGLES != gltf_primitive.mode)
				{
					Log::warning("Only triangles are supported");
					continue;
				}
				if (gltf_primitive.indices != -1)
				{
					ModelVertexBuffer indices_buffer{};
					const tinygltf::Accessor index_accessor = model.accessors[gltf_primitive.indices];
					const tinygltf::BufferView& index_buffer_view = model.bufferViews[index_accessor.bufferView];
					const tinygltf::Buffer& index_buffer = model.buffers[index_buffer_view.buffer];
					indices_buffer.data = reinterpret_cast<const void*>(&index_buffer.data.at(
						index_accessor.byteOffset + index_buffer_view.byteOffset));
					indices_buffer.count = index_accessor.count;
					switch (index_accessor.componentType)
					{
					case TINYGLTF_COMPONENT_TYPE_BYTE:
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
						break;
					case TINYGLTF_COMPONENT_TYPE_SHORT:
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
						break;
					case TINYGLTF_COMPONENT_TYPE_INT:
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
						break;
					}
					indices_buffer.element_size = tinygltf::GetComponentSizeInBytes(index_accessor.componentType);
					primitive.indices = indices_buffer;
				}

				MODEL_VERTEX_ATTRIBUTE_TYPE_FLAGS attribute_flags = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NONE;
				for (const auto& attribute : gltf_primitive.attributes)
				{
					std::string attribute_name = attribute.first;
					uint32_t accessor_index = attribute.second;
					const tinygltf::Accessor& accessor = model.accessors[accessor_index];
					const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];
					MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NONE;
					if (attribute_name == "POSITION")
					{
						type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_POSITION;
						primitive.position_min = vec3(accessor.minValues[0], accessor.minValues[1],
						                              accessor.minValues[2]);
						primitive.position_max = vec3(accessor.maxValues[0], accessor.maxValues[1],
						                              accessor.maxValues[2]);
					}
					else if (attribute_name == "NORMAL")
					{
						type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NORMAL;
					}
					else if (attribute_name == "TANGENT")
					{
						type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_TANGENT;
					}
					else if (attribute_name.starts_with("TEXCOORD_"))
					{
						type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_TEXCOORD;
					}
					else if (attribute_name.starts_with("COLOR_"))
					{
						type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_COLOR;
					}
					else if (attribute_name.starts_with("WEIGHTS_"))
					{
						type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_WEIGHTS;
					}
					else if (attribute_name.starts_with("JOINTS_"))
					{
						type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_JOINTS;
					}
					else
					{
						type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_CUSTOM;
					}
					uint32_t stride = buffer_view.byteStride;
					if (stride == 0)
					{
						stride = tinygltf::GetNumComponentsInType(accessor.type) * tinygltf::GetComponentSizeInBytes(
							accessor.componentType);
					}
					const void* data_ptr = reinterpret_cast<const void*>(&buffer.data.at(
						accessor.byteOffset + buffer_view.byteOffset));
					//first one add buffer or
					//not inside a stride so it is its own buffer

					ModelVertexBuffer mesh_buffer{};
					mesh_buffer.data = data_ptr;
					mesh_buffer.count = accessor.count;
					mesh_buffer.element_size = stride;
					mesh_buffer.type = type;
					auto it = primitive.buffers.find(type);
					if (it == primitive.buffers.end())
					{
						primitive.buffers.emplace(type, std::vector<ModelVertexBuffer>{mesh_buffer});
					}
					else
					{
						it->second.emplace_back(mesh_buffer);
					}

					attribute_flags |= type;
				}
				primitive.available_attributes |= attribute_flags;
				data.material_properties[gltf_primitive.material].used_with_attributes |= attribute_flags;
			}
		}
	}

	static TEXTURE_SAMPLER_ADDRESS_MODE gltfWrapModeToTextureSamplerAddressMode(int wrap)
	{
		switch (wrap)
		{
		case TINYGLTF_TEXTURE_WRAP_REPEAT:
			return TEXTURE_SAMPLER_ADDRESS_MODE_REPEAT;
		case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
			return TEXTURE_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
			return TEXTURE_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		}
	}

	static TEXTURE_SAMPLER_FILTER_TYPE gltfFilterModeToTextureSamplerFilter(int filter)
	{
		switch (filter)
		{
		case TINYGLTF_TEXTURE_FILTER_NEAREST:
			return TEXTURE_SAMPLER_FILTER_TYPE_NEAREST;
		default:
		case TINYGLTF_TEXTURE_FILTER_LINEAR:
			return TEXTURE_SAMPLER_FILTER_TYPE_LINEAR;
		}
	}

	void createTextureData(ModelData& data, tinygltf::Model& model, MODEL_FLAGS flags)
	{
		for (size_t i = 0; i < model.textures.size(); i++)
		{
			const tinygltf::Texture& gltf_texture = model.textures[i];
			const tinygltf::Image& gltf_image = model.images[gltf_texture.source];
			ModelTextureData texture_data{};
			if (gltf_texture.sampler != -1)
			{
				const tinygltf::Sampler& gltf_sampler = model.samplers[gltf_texture.sampler];
				if (gltf_sampler.wrapS != -1)
					texture_data.sampler_info.address_mode =
						gltfWrapModeToTextureSamplerAddressMode(gltf_sampler.wrapS);
				if (gltf_sampler.minFilter != -1)
					texture_data.sampler_info.filter = gltfFilterModeToTextureSamplerFilter(gltf_sampler.minFilter);
			}

			HB_ASSERT(gltf_image.width >= 1 && gltf_image.height >= 1, "Image Witdh or Height is 0");
			texture_data.width = gltf_image.width;
			texture_data.height = gltf_image.height;
			texture_data.channels = gltf_image.component;
			texture_data.bits_per_channels = gltf_image.bits;

			texture_data.path = gltf_image.uri;
			texture_data.data = gltf_image.image.data();
			switch (gltf_image.pixel_type)
			{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				switch (texture_data.channels)
				{
				case 1:
					texture_data.format = IMAGE_FORMAT_R8_UINT;
					break;
				case 2:
					texture_data.format = IMAGE_FORMAT_RG8_UINT;
					break;
				case 3:
					texture_data.format = IMAGE_FORMAT_RGB8_UINT;
					break;
				case 4:
					texture_data.format = IMAGE_FORMAT_RGBA8_UINT;
					break;
				}
				break;
			case TINYGLTF_COMPONENT_TYPE_BYTE:

				switch (texture_data.channels)
				{
				case 1:
					texture_data.format = IMAGE_FORMAT_R8;
					break;
				case 2:
					texture_data.format = IMAGE_FORMAT_RG8;
					break;
				case 3:
					texture_data.format = IMAGE_FORMAT_RGB8;
					break;
				case 4:
					texture_data.format = IMAGE_FORMAT_RGBA8;
					break;
				}
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				switch (texture_data.channels)
				{
				case 1:
					texture_data.format = IMAGE_FORMAT_R32_UINT;
					break;
				case 2:
					texture_data.format = IMAGE_FORMAT_RG32_UINT;
					break;
				case 3:
					texture_data.format = IMAGE_FORMAT_RGB32_UINT;
					break;
				case 4:
					texture_data.format = IMAGE_FORMAT_RGBA32_UINT;
					break;
				}
				break;
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				switch (texture_data.channels)
				{
				case 1:
					texture_data.format = IMAGE_FORMAT_R32F;
					break;
				case 2:
					texture_data.format = IMAGE_FORMAT_RG32F;
					break;
				case 3:
					texture_data.format = IMAGE_FORMAT_RGB32F;
					break;
				case 4:
					texture_data.format = IMAGE_FORMAT_RGBA32F;
					break;
				}
				texture_data.format = IMAGE_FORMAT_R32F;
				break;

			case TINYGLTF_COMPONENT_TYPE_SHORT:
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			case TINYGLTF_COMPONENT_TYPE_INT:
				Log::error("Unsupported pixel type");
				break;
			}
			data.textures_data.emplace_back(texture_data);
		}
	}

	void createMaterialData(ModelData& data, tinygltf::Model& model)
	{
		for (size_t i = 0; i < model.materials.size(); i++)
		{
			const tinygltf::Material& gltf_material = model.materials[i];
			ModelMaterialData material_data{};
			if (gltf_material.pbrMetallicRoughness.baseColorFactor.size() == 4)
			{
				material_data.properties.base_color.r = static_cast<float>(gltf_material.pbrMetallicRoughness.
					baseColorFactor[0]);
				material_data.properties.base_color.g = static_cast<float>(gltf_material.pbrMetallicRoughness.
					baseColorFactor[1]);
				material_data.properties.base_color.b = static_cast<float>(gltf_material.pbrMetallicRoughness.
					baseColorFactor[2]);
				material_data.properties.base_color.a = static_cast<float>(gltf_material.pbrMetallicRoughness.
					baseColorFactor[3]);
			}
			material_data.albedo_texture = gltf_material.pbrMetallicRoughness.baseColorTexture.index;
			material_data.metallic_roughness_texture = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.
			                                                         index;
			material_data.emmisive_texture = gltf_material.emissiveTexture.index;
			material_data.normal_texture = gltf_material.normalTexture.index;
			material_data.occlusion_texture = gltf_material.occlusionTexture.index;
			material_data.properties.has_albedo = gltf_material.pbrMetallicRoughness.baseColorTexture.index != -1;
			material_data.properties.has_metallic_roughness = gltf_material.pbrMetallicRoughness.
			                                                                metallicRoughnessTexture.index != -1;
			material_data.properties.has_emmisive = gltf_material.emissiveTexture.index != -1;
			material_data.properties.has_normal = gltf_material.normalTexture.index != -1;
			material_data.properties.has_occlusion = gltf_material.occlusionTexture.index != -1;

			material_data.double_sided = gltf_material.doubleSided;

			material_data.properties.metal_factor = gltf_material.pbrMetallicRoughness.metallicFactor;
			material_data.properties.roughness = gltf_material.pbrMetallicRoughness.roughnessFactor;
			material_data.properties.emmisive_factor = vec4(gltf_material.emissiveFactor[0],
			                                                gltf_material.emissiveFactor[1],
			                                                gltf_material.emissiveFactor[2], 0.0f);

			data.material_properties.emplace_back(material_data);
		}
	}

	void GLTFLoader::load(ModelData& data, const ModelInfo& info)
	{
		std::string err;
		std::string warn;
		//if (info.flags & MODEL_FLAG_DONT_LOAD_TEXTURES)
		//	loader.RemoveImageLoader();
		bool result;
		if (info.path.ends_with(".glb"))
		{
			result = loader.LoadBinaryFromFile(&model, &err, &warn, RESOURCE_PATH + info.path);
		}
		else
		{
			result = loader.LoadASCIIFromFile(&model, &err, &warn, RESOURCE_PATH + info.path);
		}


		if (!warn.empty())
		{
			Log::warning(warn);
		}

		if (!err.empty())
		{
			Log::error(err);
		}

		if (!result)
			Log::error("Failed to load glTF: ");

		createMaterialData(data, model);
		createTextureData(data, model, info.flags);
		createMeshData(data, model, info.flags);

		const tinygltf::Scene& scene = model.scenes[model.defaultScene];
		//root nodes
		for (size_t i = 0; i < scene.nodes.size(); ++i)
		{
			assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
			processNodes(data, model, model.nodes[scene.nodes[i]], data.nodes);
		}
	}

	GLTFLoader::~GLTFLoader()
	{
	}
}
