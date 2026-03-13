#include "DefaultModelParser.h"

#include "core/Application.h"
#include "core/interface/ImageInterface.h""
#include "core/resource/Mesh.h"
#include "core/resource/Model.h"

namespace HBE
{
    DefaultModelParser::DefaultModelParser(DefaultModelParserInfo info) : context(*Application::instance->getContext())
    {
        this->parser_info = info;
    }

    MeshHandle DefaultModelParser::createMesh(const ModelPrimitiveData& data, const ModelInfo model_info)
    {
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
        if (position_it != data.buffers.end())
        {
            VertexAttributeInfo attribute_info{};
            attribute_info.location = 0;
            attribute_info.size = sizeof(vec3);
            attribute_info.flags = VERTEX_ATTRIBUTE_FLAG_NONE;
            attribute_infos.emplace_back(attribute_info);
            positions = std::vector<vec3>(reinterpret_cast<const vec3*>(position_it->second[0].data),
                                          reinterpret_cast<const vec3*>(position_it->second[0].data) + position_it->second[0].count);
        }
        if (uv_it != data.buffers.end())
        {
            VertexAttributeInfo attribute_info{};
            attribute_info.location = 1;
            attribute_info.size = sizeof(vec2);
            attribute_info.flags = VERTEX_ATTRIBUTE_FLAG_NONE;
            attribute_infos.emplace_back(attribute_info);
            uvs = std::vector<vec2>(reinterpret_cast<const vec2*>(uv_it->second[0].data),
                                    reinterpret_cast<const vec2*>(uv_it->second[0].data) + uv_it->second[0].count);
        }
        if (normal_it != data.buffers.end())
        {
            VertexAttributeInfo attribute_info{};
            attribute_info.location = 2;
            attribute_info.size = sizeof(vec3);
            attribute_info.flags = VERTEX_ATTRIBUTE_FLAG_NONE;
            attribute_infos.emplace_back(attribute_info);
            normals = std::vector<vec3>(reinterpret_cast<const vec3*>(normal_it->second[0].data),
                                        reinterpret_cast<const vec3*>(normal_it->second[0].data) + normal_it->second[0].count);
        }


        mesh_info.attribute_infos = attribute_infos.data();
        mesh_info.attribute_info_count = attribute_infos.size();
        MeshHandle mesh_handle;
        context.createMesh(mesh_handle, mesh_info);
        switch (data.indices.element_size)
        {
        case 2:
            context.setMeshVertexIndices16(mesh_handle, reinterpret_cast<const uint16_t*>(data.indices.data), data.indices.count);
            indices16 = std::vector<uint16_t>(reinterpret_cast<const uint16_t*>(data.indices.data),
                                              reinterpret_cast<const uint16_t*>(data.indices.data) + data.indices.count);
            break;
        case 4:
            context.setMeshVertexIndices(mesh_handle, reinterpret_cast<const uint32_t*>(data.indices.data), data.indices.count);
            indices32 = std::vector<uint32_t>(reinterpret_cast<const uint32_t*>(data.indices.data),
                                              reinterpret_cast<const uint32_t*>(data.indices.data) + data.indices.count);
            break;
        }

        if (position_it != data.buffers.end())
        {
            context.setMeshVertexBuffer(mesh_handle, 0, position_it->second[0].data, position_it->second[0].count);
        }
        if (uv_it != data.buffers.end())
        {
            context.setMeshVertexBuffer(mesh_handle, 1, uv_it->second[0].data, uv_it->second[0].count);
        }
        if (normal_it != data.buffers.end())
        {
            context.setMeshVertexBuffer(mesh_handle, 2, normal_it->second[0].data, normal_it->second[0].count);
        }


        return mesh_handle;
    }

    PipelineInstanceHandle DefaultModelParser::createMaterial(const ModelMaterialData& materialData,
                                                              ImageHandle* textures)
    {
        PipelineInstanceHandle instance_handle;
        PipelineInstanceInfo instance_info{};
        instance_info.pipeline_handle = materialData.double_sided
                                            ? parser_info.graphic_pipeline_2_sided
                                            : parser_info.graphic_pipeline;

        context.createPipelineInstance(instance_handle, instance_info);

        auto texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_ALBEDO);
        std::string texture_name = texture_type_it->second;
        MODEL_TEXTURE_TYPE texture_type = texture_type_it->first;
        uint32_t binding = 0;
        uint32_t material_binding = 0;
        context.getPipelineInstanceBindingFromString(instance_handle, texture_name.c_str(), binding);
        context.getPipelineInstanceBindingFromString(instance_handle, parser_info.material_property_name.c_str(), material_binding);
        if (texture_type_it != parser_info.texture_names.end() && materialData.albedo_texture != -1)
            context.setPipelineInstanceImage(instance_handle, binding, textures[materialData.albedo_texture],0,-1);
        texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_METALLIC_ROUGHNESS);
        if (texture_type_it != parser_info.texture_names.end() && materialData.metallic_roughness_texture != -1)
            context.setPipelineInstanceImage(instance_handle, binding, textures[materialData.metallic_roughness_texture],0,-1);
        texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_EMMISIVE);
        if (texture_type_it != parser_info.texture_names.end() && materialData.emmisive_texture != -1)
            context.setPipelineInstanceImage(instance_handle, binding, textures[materialData.emmisive_texture],0,-1);
        texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_NORMAL);
        if (texture_type_it != parser_info.texture_names.end() && materialData.normal_texture != -1)
            context.setPipelineInstanceImage(instance_handle, binding, textures[materialData.normal_texture],0,-1);
        texture_type_it = parser_info.texture_names.find(MODEL_TEXTURE_TYPE_OCCLUSION);
        if (texture_type_it != parser_info.texture_names.end() && materialData.occlusion_texture != -1)
            context.setPipelineInstanceImage(instance_handle, binding, textures[materialData.occlusion_texture],0,-1);

        context.setPipelineInstanceUniform(instance_handle,material_binding, &materialData.properties,-1);

        return instance_handle;
    }

    ImageHandle DefaultModelParser::createTexture(const ModelTextureData& data)
    {
        ImageInfo texture_info{};
        texture_info.width = data.width;
        texture_info.height = data.height;
        texture_info.optional_data = data.data;
        texture_info.data_format = data.format;
        texture_info.format = data.format;
        texture_info.flags = IMAGE_FLAG_NONE;
        texture_info.sampler_info = data.sampler_info;
        ImageHandle handle;
        context.createImage(handle, texture_info);
    }

    MeshAccelerationStructureHandle DefaultModelParser::createMeshAccelerationStructure(Mesh* mesh, int mesh_index)
    {
        MeshAccelerationStructureInfo acceleration_structure_info{};
        acceleration_structure_info.mesh_handle = mesh->getHandle();
        MeshAccelerationStructureHandle acceleration_structure_handle;
        context.createMeshAccelerationStructure(acceleration_structure_handle, acceleration_structure_info);
        return acceleration_structure_handle;
    }
}
