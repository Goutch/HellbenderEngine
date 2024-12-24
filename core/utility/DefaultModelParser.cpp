#include "core/resource/GraphicPipelineInstance.h"
#include "DefaultModelParser.h"
#include "core/resource/Resources.h"
#include "core/resource/Texture.h"
#include "core/resource/Mesh.h"
#include "core/resource/Model.h"

namespace HBE
{
	DefaultModelParser::DefaultModelParser(DefaultModelParserInfo info)
	{
		this->parser_info = info;
	}

	Mesh* DefaultModelParser::createMesh(const ModelPrimitiveData& data, const ModelInfo model_info)
	{
		MeshInfo mesh_info{};
		mesh_info.flags = MESH_FLAG_NONE;
		mesh_info.flags |= (model_info.flags & MODEL_FLAG_USED_IN_RAYTRACING) == MODEL_FLAG_USED_IN_RAYTRACING
			                   ? MESH_FLAG_USED_IN_RAYTRACING
			                   : MESH_FLAG_NONE;

		std::vector<VertexAttributeInfo> attribute_infos;
		auto position_it = data.buffers.find(MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_POSITION);
		auto normal_it = data.buffers.find(MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NORMAL);
		auto uv_it = data.buffers.find(MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_TEXCOORD);
		if (position_it != data.buffers.end())
		{
			VertexAttributeInfo attribute_info{};
			attribute_info.location = 0;
			attribute_info.size = sizeof(vec3);
			attribute_info.flags = VERTEX_ATTRIBUTE_FLAG_NONE;
			attribute_infos.emplace_back(attribute_info);
		}
		if (uv_it != data.buffers.end())
		{
			VertexAttributeInfo attribute_info{};
			attribute_info.location = 1;
			attribute_info.size = sizeof(vec2);
			attribute_info.flags = VERTEX_ATTRIBUTE_FLAG_NONE;
			attribute_infos.emplace_back(attribute_info);
		}
		if (normal_it != data.buffers.end())
		{
			VertexAttributeInfo attribute_info{};
			attribute_info.location = 2;
			attribute_info.size = sizeof(vec3);
			attribute_info.flags = VERTEX_ATTRIBUTE_FLAG_NONE;
			attribute_infos.emplace_back(attribute_info);
		}


		mesh_info.attribute_infos = attribute_infos.data();
		mesh_info.attribute_info_count = attribute_infos.size();
		Mesh* mesh_ptr = Resources::createMesh(mesh_info);
		switch (data.indices.element_size)
		{
		case 2:
			mesh_ptr->setVertexIndices(reinterpret_cast<const uint16_t*>(data.indices.data), data.indices.count);
			break;
		case 4:
			mesh_ptr->setVertexIndices(reinterpret_cast<const uint32_t*>(data.indices.data), data.indices.count);
			break;
		}

		if (position_it != data.buffers.end())
		{
			mesh_ptr->setBuffer(0, position_it->second[0].data, position_it->second[0].count);
		}
		if (uv_it != data.buffers.end())
		{
			mesh_ptr->setBuffer(1, uv_it->second[0].data, uv_it->second[0].count);
		}
		if (normal_it != data.buffers.end())
		{
			mesh_ptr->setBuffer(2, normal_it->second[0].data, normal_it->second[0].count);
		}

		return mesh_ptr;
	}

	GraphicPipelineInstance* DefaultModelParser::createMaterial(const ModelMaterialData& materialData,
	                                                            HBE::Texture** textures)
	{
		GraphicPipelineInstanceInfo instance_info{};
		instance_info.graphic_pipeline = materialData.double_sided
			                                 ? parser_info.graphic_pipeline_2_sided
			                                 : parser_info.graphic_pipeline;
		instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_EMPTY_DESCRIPTOR_ALLOWED;

		GraphicPipelineInstance* instance_ptr = Resources::createGraphicPipelineInstance(instance_info);

		auto texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_ALBEDO);
		if (texture_type_it != parser_info.texture_names.end() && materialData.albedo_texture != -1)
			instance_ptr->setTexture(texture_type_it->second, textures[materialData.albedo_texture]);
		texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_METALLIC_ROUGHNESS);
		if (texture_type_it != parser_info.texture_names.end() && materialData.metallic_roughness_texture != -1)
			instance_ptr->setTexture(texture_type_it->second, textures[materialData.metallic_roughness_texture]);
		texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_EMMISIVE);
		if (texture_type_it != parser_info.texture_names.end() && materialData.emmisive_texture != -1)
			instance_ptr->setTexture(texture_type_it->second, textures[materialData.emmisive_texture]);
		texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_NORMAL);
		if (texture_type_it != parser_info.texture_names.end() && materialData.normal_texture != -1)
			instance_ptr->setTexture(texture_type_it->second, textures[materialData.normal_texture]);
		texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_OCCLUSION);
		if (texture_type_it != parser_info.texture_names.end() && materialData.occlusion_texture != -1)
			instance_ptr->setTexture(texture_type_it->second, textures[materialData.occlusion_texture]);

		instance_ptr->setUniform(parser_info.material_property_name, &materialData.properties);

		return instance_ptr;
	}
}
