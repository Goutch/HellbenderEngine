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

    PipelineInstanceHandle PipelineInstance::getHandle() const
    {
        return handle;
    }

    PipelineInstanceHandle& PipelineInstance::getHandleRef()
    {
        return handle;
    }

    void PipelineInstance::setUniform(uint32_t binding, const void* data)
    {
    }

    void PipelineInstance::setUniform(const char* name, const void* data)
    {
        uint32_t binding = 0;
        context.getPipelineInstanceBindingFromString(handle, name, binding);
        setUniform(binding, data);
    }

    void PipelineInstance::setImage(uint32_t binding, ImageHandle image, uint32_t mip_level)
    {
    }

    void PipelineInstance::setImageArray(uint32_t binding, ImageHandle* images, uint32_t images_count, uint32_t mip_level)
    {
    }

    void PipelineInstance::setImage(const char* name, ImageHandle image, uint32_t mip_level)
    {
        uint32_t binding = 0;
        context.getPipelineInstanceBindingFromString(handle, name, binding);
        setImage(binding, image, mip_level);
    }

    void PipelineInstance::setImageArray(const char* name, ImageHandle* images, uint32_t images_count, uint32_t mip_level)
    {
        uint32_t binding = 0;
        context.getPipelineInstanceBindingFromString(handle, name, binding);
        setImageArray(binding, images, images_count, mip_level);
    }

    void PipelineInstance::setStorageBufferArray(uint32_t binding, StorageBufferHandle* buffers, uint32_t count)
    {
    }

    void PipelineInstance::setStorageBuffer(uint32_t binding, StorageBufferHandle buffer, size_t count, size_t offset)
    {
    }

    void PipelineInstance::setStorageBuffer(const char* name, StorageBufferHandle buffer, size_t count, size_t offset)
    {
        uint32_t binding = 0;
        context.getPipelineInstanceBindingFromString(handle, name, binding);
        setStorageBuffer(binding, buffer, count, offset);
    }

    void PipelineInstance::setStorageBufferArray(const char* name, StorageBufferHandle* buffers, uint32_t count)
    {
        uint32_t binding = 0;
        context.getPipelineInstanceBindingFromString(handle, name, binding);
        setStorageBufferArray(binding, buffers, count);
    }

    void PipelineInstance::setTexelBuffer(uint32_t binding, TexelBufferHandle buffer)
    {
    }

    void PipelineInstance::setTexelBufferArray(uint32_t binding, TexelBufferHandle* buffers, uint32_t count)
    {
    }

    void PipelineInstance::setTexelBuffer(const char* name, TexelBufferHandle buffer)
    {
        uint32_t binding = 0;
        context.getPipelineInstanceBindingFromString(handle, name, binding);
        setTexelBuffer(binding, buffer);
    }

    void PipelineInstance::setTexelBufferArray(const char* name, TexelBufferHandle* buffers, uint32_t count)
    {
        uint32_t binding = 0;
        context.getPipelineInstanceBindingFromString(handle, name, binding);
        setTexelBufferArray(binding, buffers, count);
    }

    void PipelineInstance::setAccelerationStructure(uint32_t binding, RootAccelerationStructureHandle accelerationStructure)
    {
    }

    void PipelineInstance::setAccelerationStructure(const char* name, RootAccelerationStructureHandle accelerationStructure)
    {
        uint32_t binding = 0;
        context.getPipelineInstanceBindingFromString(handle, name, binding);
        setAccelerationStructure(binding, accelerationStructure);
    }
}
