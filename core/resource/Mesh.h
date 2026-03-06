#pragma once

#include "Core.h"
#include <vector>
#include "Resource.h"
#include "unordered_map"
#include "string"
#include "core/interface/MeshInterface.h"
#include "core/scene/components/MeshRenderer.h"

namespace HBE
{
    class StorageBuffer;
    typedef uint32_t

    MESH_FLAGS;

    enum MESH_FLAG
    {
        MESH_FLAG_NONE = 0,
        MESH_FLAG_USED_IN_RAYTRACING = 1 << 0,
        MESH_FLAG_USED_AS_STORAGE_BUFFER = 1 << 1,
    };

    enum MESH_TOPOLOGY
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

    class HB_API Mesh
    {
        Context& context;
        MeshHandle handle = HBE_NULL_HANDLE;

        enum INDICES_TYPE
        {
            INDICES_TYPE_NONE,
            INDICES_TYPE_UINT16,
            INDICES_TYPE_UINT32,
        };

        uint32_t vertex_count = 0;
        uint32_t index_count = 0;
        uint32_t instance_count = 1;
        INDICES_TYPE indices_type = INDICES_TYPE_NONE;
        std::vector<VertexAttributeInfo> attributes_locations;

    public:
        Mesh();
        explicit Mesh(const MeshInfo& mesh_info);
        ~Mesh();
        void alloc(const MeshInfo& mesh_info);
        void release();
        MeshHandle getHandle();

        void setVertexIndices(const uint32_t* vertices, size_t count);
        void setVertexIndices(const uint16_t* vertices, size_t count);
        void setBuffer(uint32_t location, const void* vertices, size_t count);
        void setInstanceBuffer(uint32_t location, const void* data, size_t count);

        void load(std::string path);

        void loadAsync(std::string path);

    };
}
