#include "PipelineInstance.h"

#include "core/Application.h"

namespace HBE
{
    PipelineInstance::PipelineInstance() : context(*Application::instance->getContext())
    {
    }

    PipelineInstance::PipelineInstance(const PipelineInstanceInfo& info): context(*Application::instance->getContext())
    {
        alloc(info);
    }

    PipelineInstance::~PipelineInstance()
    {
        release();
    }

    void PipelineInstance::alloc(const PipelineInstanceInfo& info)
    {
        context.createPipelineInstance(handle, info);
    }

    void PipelineInstance::release()
    {
        if (handle != HBE_NULL_HANDLE)
            context.releasePipelineInstance(handle);
    }

    PipelineInstanceHandle PipelineInstance::getHandle()
    {
        return handle;
    }

    void PipelineInstance::setUniform(uint32_t binding, const void* data)
    {
        context.
    }

    void PipelineInstance::setUniform(const char* name, const void* data)
    {
    }

    void PipelineInstance::setImage(uint32_t binding, ImageHandle image, uint32_t mip_level)
    {
    }

    void PipelineInstance::setImageArray(uint32_t binding, ImageHandle* images, uint32_t images_count, uint32_t mip_level)
    {
    }

    void PipelineInstance::setImage(const char* name, ImageHandle image, uint32_t mip_level)
    {
    }

    void PipelineInstance::setImageArray(const char* name, ImageHandle* images, uint32_t images_count, uint32_t mip_level)
    {
    }

    void PipelineInstance::setStorageBuffer(uint32_t binding, StorageBufferHandle buffer, size_t count, size_t offset, int32_t frame)
    {
    }

    void PipelineInstance::setStorageBufferArray(uint32_t binding, StorageBufferHandle* buffers, uint32_t count)
    {
    }

    void PipelineInstance::setStorageBuffer(const char* name, StorageBufferHandle buffer, size_t count, size_t offset)
    {
    }

    void PipelineInstance::setStorageBufferArray(const std::string& name, StorageBufferHandle* buffers, uint32_t count)
    {
    }

    void PipelineInstance::setTexelBuffer(uint32_t binding, TexelBufferHandle buffer)
    {
    }

    void PipelineInstance::setTexelBufferArray(uint32_t binding, TexelBufferHandle* buffers, uint32_t count)
    {
    }

    void PipelineInstance::setTexelBuffer(const char* name, TexelBufferHandle buffer)
    {
    }

    void PipelineInstance::setTexelBufferArray(const char* name, TexelBufferHandle* buffers, uint32_t count)
    {
    }

    void PipelineInstance::setAccelerationStructure(uint32_t binding, RootAccelerationStructureHandle accelerationStructure)
    {
    }

    void PipelineInstance::setAccelerationStructure(const std::string& name, RootAccelerationStructureHandle accelerationStructure)
    {
    }
}
