#include "Mesh.h"

#include "core/Application.h"

namespace HBE
{
    Mesh::Mesh(): context(*Application::instance->getContext())
    {
    }

    Mesh::Mesh(const MeshInfo& mesh_info): context(*Application::instance->getContext())
    {
        alloc(mesh_info);
    }

    Mesh::~Mesh()
    {
        release();
    }

    void Mesh::alloc(const MeshInfo& mesh_info)
    {
        context.createMesh(handle, mesh_info);
    }

    bool Mesh::allocated()
    {
        return handle != HBE_NULL_HANDLE;
    }

    void Mesh::release()
    {
        if (allocated())
            context.releaseMesh(handle);
        handle = HBE_NULL_HANDLE;
    }

    MeshHandle& Mesh::getHandleRef()
    {
        return handle;
    }

    MeshHandle Mesh::getHandle()
    {
        return handle;
    }

    void Mesh::setVertexIndices(const uint32_t* indices, size_t count)
    {
        context.setMeshVertexIndices(handle, indices, count);
    }

    void Mesh::setVertexIndices(const uint16_t* indices, size_t count)
    {
        context.setMeshVertexIndices16(handle, indices, count);
    }

    void Mesh::setBuffer(uint32_t location, const void* vertices, size_t count)
    {
        context.setMeshVertexBuffer(handle, location, vertices, count);
    }

    void Mesh::setInstanceBuffer(uint32_t location, const void* data, size_t count)
    {
        context.setMeshInstanceBuffer(handle, location, data, count);
    }
}
