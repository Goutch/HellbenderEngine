#pragma once

#include <core/resource/StorageBuffer.h>
#include "core/resource/raytracing/RaytracingPipelineInstance.h"

namespace HBE
{
    class VK_Context;
    class VK_Renderer;

    class VK_RaytracingPipeline;

    class VK_PipelineLayout;

    class VK_PipelineDescriptors;

    class VK_Fence;

    class VK_RaytracingPipelineInstance : public RaytracingPipelineInstance
    {
        VK_RaytracingPipeline* pipeline;
        VK_PipelineDescriptors* descriptors = nullptr;
        VK_Fence* fence;

    public:
        VK_RaytracingPipelineInstance(VK_Context* context, const RaytracingPipelineInstanceInfo& info);

        ~VK_RaytracingPipelineInstance();

        const VK_RaytracingPipeline* getPipeline() const;

        //---------------------------IPipelineInstance Implementation---------------------------
        void bind() const override;

        void unbind() const override;

        void setUniform(uint32_t binding, const void* data, int32_t frame) override;

        void setUniform(const std::string& name, const void* data, int32_t frame) override;

        void setImage(uint32_t binding, const Image* texture, int32_t frame, uint32_t mip_level) override;

        void setImage(const std::string& name, const Image* texture, int32_t frame, uint32_t mip_level) override;

        void setImageArray(uint32_t binding, Image** texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;

        void setImageArray(const std::string& name, Image** texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;

        void setStorageBuffer(uint32_t binding, StorageBuffer* buffer, size_t count, size_t offset, int32_t frame) override;

        void setStorageBufferArray(uint32_t binding, StorageBuffer** buffer, uint32_t count, int32_t frame) override;

        void setStorageBuffer(const std::string& name, StorageBuffer* buffer, size_t count, size_t offset, int32_t frame) override;

        void setStorageBufferArray(const std::string& name, StorageBuffer** buffer, uint32_t count, int32_t frame) override;

        void setTexelBuffer(uint32_t binding, TexelBuffer* buffer, int32_t frame) override;

        void setTexelBufferArray(uint32_t binding, TexelBuffer** buffers, uint32_t count, int32_t frame) override;

        void setTexelBuffer(const std::string& name, TexelBuffer* buffer, int32_t frame) override;

        void setTexelBufferArray(const std::string& name, TexelBuffer** buffers, uint32_t count, int32_t frame) override;

        void setAccelerationStructure(uint32_t binding, const RootAccelerationStructure* accelerationStructure, int32_t frame) override;

        void setAccelerationStructure(const std::string& name, const RootAccelerationStructure* accelerationStructure, int32_t frame) override;
    };
}
