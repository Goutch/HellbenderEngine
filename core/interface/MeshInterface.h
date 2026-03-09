#pragma once
#include "HBETypes.h"
#include "core/interface/AllocatorInterface.h"
#include "data-structure/Handle.h"

namespace HBE
{
    typedef uint8_t VERTEX_ATTRIBUTE_FLAGS;

    enum VERTEX_ATTRIBUTE_FLAG :uint8_t
    {
        VERTEX_ATTRIBUTE_FLAG_NONE = 0,
        VERTEX_ATTRIBUTE_FLAG_PER_INSTANCE = 1 << 0, /// Disable device local memory and skip staging buffer. May result in slower read in shader.
    };

    enum VERTEX_TOPOLOGY :uint8_t
    {
        VERTEX_TOPOLOGY_POINT_LIST = 0,
        VERTEX_TOPOLOGY_LINE_LIST = 1,
        VERTEX_TOPOLOGY_LINE_STRIP = 2,
        VERTEX_TOPOLOGY_TRIANGLE_LIST = 3,
        VERTEX_TOPOLOGY_TRIANGLE_STRIP = 4,
        VERTEX_TOPOLOGY_TRIANGLE_FAN = 5,
        VERTEX_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
        VERTEX_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
        VERTEX_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
        VERTEX_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
        VERTEX_TOPOLOGY_PATCH_LIST = 10,
    };

    struct VertexAttributeInfo
    {
        //max value invalid
        uint32_t location = std::numeric_limits<uint32_t>::max();
        uint32_t size = 0;
        VERTEX_ATTRIBUTE_FLAGS flags = VERTEX_ATTRIBUTE_FLAG_NONE;
        MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
    };

    const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION3D = {0, sizeof(vec3), VERTEX_ATTRIBUTE_FLAG_NONE};
    const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION3D_NORMAL_INTERLEAVED = {0, sizeof(vec3) + sizeof(vec3), VERTEX_ATTRIBUTE_FLAG_NONE};
    const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED = {0, sizeof(vec3) + sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};
    const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION3D_NORMAL_UV_INTERLEAVED = {0, sizeof(vec3) + sizeof(vec3) + sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};

    const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION2D = {0, sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};
    const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION2D_NORMAL_INTERLEAVED = {0, sizeof(vec2) + sizeof(vec3), VERTEX_ATTRIBUTE_FLAG_NONE};
    const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION2D_UV_INTERLEAVED = {0, sizeof(vec2) + sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};
    const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION2D_NORMAL_UV_INTERLEAVED = {0, sizeof(vec2) + sizeof(vec3) + sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};


    typedef uint8_t MESH_FLAGS;
    enum MESH_FLAG : uint8_t
    {
        MESH_FLAG_NONE = 0,
        MESH_FLAG_USED_IN_RAYTRACING = 1 << 0,
        MESH_FLAG_USED_AS_STORAGE_BUFFER = 1 << 1,
    };

    enum MESH_TOPOLOGY : uint8_t
    {
        MESH_TOPOLOGY_TRIANGLE_LIST,
        MESH_TOPOLOGY_TRIANGLE_STRIP,
        MESH_TOPOLOGY_TRIANGLE_FAN,
        MESH_TOPOLOGY_LINE_LIST,
        MESH_TOPOLOGY_LINE_STRIP,
        MESH_TOPOLOGY_POINT_LIST,
    };

    struct MeshInfo
    {
        const VertexAttributeInfo* attribute_infos = nullptr;
        size_t attribute_info_count = 0;
        MESH_FLAGS flags = MESH_FLAG_NONE;
    };
    using MeshHandle = Handle;
}
