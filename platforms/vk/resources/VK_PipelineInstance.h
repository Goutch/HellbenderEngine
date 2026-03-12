#pragma once
#include <array>
#include <queue>

#include "Core.h"
#include "Event.h"
#include "core/interface/MeshAccelerationStructureInterface.h"
#include "core/interface/BufferInterface.h"
#include "core/interface/PipelineInstanceInterface.h"
#include "core/interface/ROOTAccelerationStructureInterface.h"
#include "platforms/vk/VK_Buffer.h"
#include "platforms/vk/VK_CommandPool.h"

namespace HBE
{
    class VK_PipelineLayout;
    class VK_TopLevelAccelerationStructure;
    class VK_Context;

    struct VariableDescriptorSet
    {
        uint32_t binding;
        uint32_t count = 1;
        VkDescriptorType type;
    };

    struct DescriptorPool
    {
        VkDescriptorPool handle;
        std::vector<VkDescriptorSet> descriptor_set_handles; //frame0 set0 |frame0 set1 | frame1 set0 | frame1 set1| frame2 set0 | frame2 set1
        std::array<std::vector<VkWriteDescriptorSet>, MAX_FRAMES_IN_FLIGHT> writes;
        std::vector<VariableDescriptorSet> variable_descriptor_sets; //frame0 set0 |frame0 set1 | frame1 set0 | frame1 set1| frame2 set0 | frame2 set1
    };

    class VK_PipelineInstance
    {
        bool empty_descriptor_allowed;
        VK_Context* context = nullptr;
        PIPELINE_INSTANCE_TYPE pipeline_type;
        Handle pipeline_handle;;

        const VK_PipelineLayout* pipeline_layout = nullptr;

        DescriptorPool descriptor_pool;
        DescriptorPool temp_descriptor_pool;

        std::array<std::vector<VK_Buffer>, MAX_FRAMES_IN_FLIGHT> uniform_buffers;
        std::queue<VK_Buffer> uniform_buffer_pool;

        std::queue<std::pair<uint32_t, VkDescriptorPool>> old_descriptor_pools;

        mutable bool bound = false;
        event_subscription_id on_frame_change_subscription_id;
    public:
        VK_PipelineInstance() = default;
        ~VK_PipelineInstance() = default;
        VK_PipelineInstance(const VK_PipelineInstance&) = delete;
        VK_PipelineInstance& operator=(const VK_PipelineInstance&) = delete;

        PIPELINE_INSTANCE_TYPE getType();
        void alloc(VK_Context* context, const PipelineInstanceInfo& info);

        void release();

        void onFrameEnd(uint32_t frame);

        Handle getPipeline();

        void bind() const;

        uint32_t getBinding(const std::string& name) const;

        void resetPool(DescriptorPool& pool);

        void bind(VkCommandBuffer command_buffer, uint32_t frame) const;

        void unbind() const;

        void setUniform(uint32_t binding, const void* data, int32_t frame);

        void setImage(uint32_t binding, ImageHandle image,  uint32_t mip_level,int32_t frame);

        void setImageArray(uint32_t binding, ImageHandle* images, uint32_t image_count, int32_t mip_level,int32_t frame);

        void setTexelBuffer(uint32_t binding, TexelBufferHandle buffer, int32_t frame);

        void setTexelBufferArray(uint32_t binding, TexelBufferHandle* buffers, uint32_t buffer_count, int32_t frame);

        void setAccelerationStructure(uint32_t binding, RootAccelerationStructureHandle acceleration_structure, int32_t frame);

        void setStorageBuffer(uint32_t binding, StorageBufferHandle buffer, size_t count, size_t offset, int32_t frame);

        void setStorageBufferArray(uint32_t binding, StorageBufferHandle* buffers, uint32_t count, int32_t frame);

    private:
        void copyDescriptorSets(DescriptorPool& from, DescriptorPool& to, int64_t frame);

        void createDescriptorWrites(DescriptorPool& pool);

        void createDescriptorPool(DescriptorPool& pool);

        void createVariableSizeDescriptors(uint32_t binding, VkDescriptorType descriptor_type, uint32_t count, int32_t frame);
    };
}
