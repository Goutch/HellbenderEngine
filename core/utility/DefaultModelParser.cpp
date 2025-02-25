#include "core/resource/RasterizationPipelineInstance.h"
#include "DefaultModelParser.h"
#include "core/resource/Resources.h"
#include "core/resource/Image.h"
#include "core/resource/Mesh.h"
#include "core/resource/Model.h"

namespace HBE {
	DefaultModelParser::DefaultModelParser(DefaultModelParserInfo info) {
		this->parser_info = info;
	}

	Mesh *DefaultModelParser::createMesh(const ModelPrimitiveData &data, const ModelInfo model_info) {
		std::vector<uint32_t> indices32;
		std::vector<uint16_t> indices16;
		std::vector<vec3> positions;
		std::vector<vec2> uvs;
		std::vector<vec3> normals;
		MeshInfo mesh_info{};
		mesh_info.flags = parser_info.additional_mesh_flags;
		mesh_info.flags |= (model_info.flags & MODEL_FLAG_USED_IN_RAYTRACING) == MODEL_FLAG_USED_IN_RAYTRACING
		                   ? MESH_FLAG_USED_IN_RAYTRACING
		                   : MESH_FLAG_NONE;

		std::vector<VertexAttributeInfo> attribute_infos;
		auto position_it = data.buffers.find(MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_POSITION);
		auto normal_it = data.buffers.find(MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NORMAL);
		auto uv_it = data.buffers.find(MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_TEXCOORD);
		if (position_it != data.buffers.end()) {
			VertexAttributeInfo attribute_info{};
			attribute_info.location = 0;
			attribute_info.size = sizeof(vec3);
			attribute_info.flags = VERTEX_ATTRIBUTE_FLAG_NONE;
			attribute_infos.emplace_back(attribute_info);
			positions = std::vector<vec3>(reinterpret_cast<const vec3 *>(position_it->second[0].data),
			                              reinterpret_cast<const vec3 *>(position_it->second[0].data) + position_it->second[0].count);
		}
		if (uv_it != data.buffers.end()) {
			VertexAttributeInfo attribute_info{};
			attribute_info.location = 1;
			attribute_info.size = sizeof(vec2);
			attribute_info.flags = VERTEX_ATTRIBUTE_FLAG_NONE;
			attribute_infos.emplace_back(attribute_info);
			uvs = std::vector<vec2>(reinterpret_cast<const vec2 *>(uv_it->second[0].data),
			                        reinterpret_cast<const vec2 *>(uv_it->second[0].data) + uv_it->second[0].count);
		}
		if (normal_it != data.buffers.end()) {
			VertexAttributeInfo attribute_info{};
			attribute_info.location = 2;
			attribute_info.size = sizeof(vec3);
			attribute_info.flags = VERTEX_ATTRIBUTE_FLAG_NONE;
			attribute_infos.emplace_back(attribute_info);
			normals = std::vector<vec3>(reinterpret_cast<const vec3 *>(normal_it->second[0].data),
			                            reinterpret_cast<const vec3 *>(normal_it->second[0].data) + normal_it->second[0].count);
		}


		mesh_info.attribute_infos = attribute_infos.data();
		mesh_info.attribute_info_count = attribute_infos.size();
		Mesh *mesh_ptr = Resources::createMesh(mesh_info);
		switch (data.indices.element_size) {
			case 2:
				mesh_ptr->setVertexIndices(reinterpret_cast<const uint16_t *>(data.indices.data), data.indices.count);
				indices16 = std::vector<uint16_t>(reinterpret_cast<const uint16_t *>(data.indices.data),
				                                  reinterpret_cast<const uint16_t *>(data.indices.data) + data.indices.count);
				break;
			case 4:
				mesh_ptr->setVertexIndices(reinterpret_cast<const uint32_t *>(data.indices.data), data.indices.count);
				indices32 = std::vector<uint32_t>(reinterpret_cast<const uint32_t *>(data.indices.data),
				                                  reinterpret_cast<const uint32_t *>(data.indices.data) + data.indices.count);
				break;
		}

		if (position_it != data.buffers.end()) {
			mesh_ptr->setBuffer(0, position_it->second[0].data, position_it->second[0].count);
		}
		if (uv_it != data.buffers.end()) {
			mesh_ptr->setBuffer(1, uv_it->second[0].data, uv_it->second[0].count);
		}
		if (normal_it != data.buffers.end()) {
			mesh_ptr->setBuffer(2, normal_it->second[0].data, normal_it->second[0].count);
		}


		return mesh_ptr;
	}

	RasterizationPipelineInstance *DefaultModelParser::createMaterial(const ModelMaterialData &materialData,
	                                                                  HBE::Image **textures) {
		RasterizationPipelineInstanceInfo instance_info{};
		instance_info.rasterization_pipeline = materialData.double_sided
		                                 ? parser_info.graphic_pipeline_2_sided
		                                 : parser_info.graphic_pipeline;

		RasterizationPipelineInstance *instance_ptr = Resources::createRasterizationPipelineInstance(instance_info);

		auto texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_ALBEDO);
		if (texture_type_it != parser_info.texture_names.end() && materialData.albedo_texture != -1)
			instance_ptr->setImage(texture_type_it->second, textures[materialData.albedo_texture]);
		texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_METALLIC_ROUGHNESS);
		if (texture_type_it != parser_info.texture_names.end() && materialData.metallic_roughness_texture != -1)
			instance_ptr->setImage(texture_type_it->second, textures[materialData.metallic_roughness_texture]);
		texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_EMMISIVE);
		if (texture_type_it != parser_info.texture_names.end() && materialData.emmisive_texture != -1)
			instance_ptr->setImage(texture_type_it->second, textures[materialData.emmisive_texture]);
		texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_NORMAL);
		if (texture_type_it != parser_info.texture_names.end() && materialData.normal_texture != -1)
			instance_ptr->setImage(texture_type_it->second, textures[materialData.normal_texture]);
		texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_OCCLUSION);
		if (texture_type_it != parser_info.texture_names.end() && materialData.occlusion_texture != -1)
			instance_ptr->setImage(texture_type_it->second, textures[materialData.occlusion_texture]);

		instance_ptr->setUniform(parser_info.material_property_name, &materialData.properties);

		return instance_ptr;
	}

	Image *DefaultModelParser::createTexture(const ModelTextureData &data) {
		ImageInfo texture_info{};
		texture_info.width = data.width;
		texture_info.height = data.height;
		texture_info.data = data.data;
		texture_info.data_format = data.format;
		texture_info.format = data.format;
		texture_info.flags = IMAGE_FLAG_NONE;
		texture_info.sampler_info = data.sampler_info;
		return Resources::createImage(texture_info);
	}

	MeshAccelerationStructure *DefaultModelParser::createMeshAccelerationStructure(Mesh *mesh, int mesh_index) {
		MeshAccelerationStructureInfo acceleration_structure_info{};
		acceleration_structure_info.mesh = mesh;
		return Resources::createMeshAccelerationStructure(acceleration_structure_info);
	}

}
