#pragma once

#include "string"
#include "Core.h"
#include "core/interface/AccelerationStructureInterface.h"
#include "core/interface/PipelineInstanceInterface.h"
#include "core/interface/RasterizationPipelineInterface.h"
#include "core/interface/BufferInterface.h"

#include "core/Graphics.h"


namespace HBE
{
    class Image;

    class StorageBuffer;

    class TexelBuffer;

    class HB_API PipelineInstance
    {
        PipelineInstanceHandle handle = HBE_NULL_HANDLE;
        Context& context;

    public:
        PipelineInstance();
        explicit PipelineInstance(const PipelineInstanceInfo& info);
        ~PipelineInstance();
        void alloc(const PipelineInstanceInfo& info);
        void release();
        PipelineInstanceHandle getHandle();

        void setUniform(uint32_t binding, const void* data);
        void setUniform(const char* name, const void* data);
        void setImage(uint32_t binding, ImageHandle image, uint32_t mip_level = 0);
        void setImageArray(uint32_t binding, ImageHandle* images, uint32_t images_count, uint32_t mip_level = 0);
        void setImage(const char* name, ImageHandle image, uint32_t mip_level = 0);
        void setImageArray(const char* name, ImageHandle* images, uint32_t images_count, uint32_t mip_level = 0);
        void setStorageBuffer(uint32_t binding, StorageBufferHandle buffer, size_t count, size_t offset, int32_t frame);
        void setStorageBufferArray(uint32_t binding, StorageBufferHandle* buffers, uint32_t count);
        void setStorageBuffer(const char* name, StorageBufferHandle buffer, size_t count, size_t offset);
        void setStorageBufferArray(const std::string& name, StorageBufferHandle* buffers, uint32_t count);
        void setTexelBuffer(uint32_t binding, TexelBufferHandle buffer);
        void setTexelBufferArray(uint32_t binding, TexelBufferHandle* buffers, uint32_t count);
        void setTexelBuffer(const char* name, TexelBufferHandle buffer);
        void setTexelBufferArray(const char* name, TexelBufferHandle* buffers, uint32_t count);
        void setAccelerationStructure(uint32_t binding, RootAccelerationStructureHandle accelerationStructure);
        void setAccelerationStructure(const std::string& name, RootAccelerationStructureHandle accelerationStructure);

        //compute only
        void dispatchAsync(uint32_t size_x, uint32_t size_y, uint32_t size_z);
        bool isDispatchFinished();
        void wait();
    };
}
