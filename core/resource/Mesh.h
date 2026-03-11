#pragma once

#include "Core.h"
#include "core/Graphics.h"
#include "core/interface/MeshInterface.h"

namespace HBE
{

    class HB_API Mesh
    {
        Context& context;
        MeshHandle handle = HBE_NULL_HANDLE;



        uint32_t vertex_count = 0;
        uint32_t index_count = 0;
        uint32_t instance_count = 1;

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
    };
}
