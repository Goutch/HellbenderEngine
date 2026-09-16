#pragma once
#include <array>
#include <queue>
#include "Core.h"
#include "Event.h"
#include "core/interface/MeshAccelerationStructureInterface.h"
#include "core/interface/BufferInterface.h"
#include "core/interface/PipelineInstanceInterface.h"
#include "core/interface/ROOTAccelerationStructureInterface.h"
#include "VK_Buffer.h"
#include "platforms/vk/VK_CommandPool.h"
#include "platforms/vk/VK_DescriptorAllocator.h"

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

    class HB_API VK_PipelineInstance
    {
        VK_Context* context = nullptr;
        Handle pipeline_handle = HBE_NULL_HANDLE;
        const VK_PipelineLayout* pipeline_layout = nullptr;
        event_subscription_id on_frame_change_subscription_id = HBE_NULL_HANDLE;
		RawVector<DescriptorSetAllocation> descriptor_allocations; //frame0 set0 |frame0 set1 | frame1 set0 | frame1 set1| frame2 set0 | frame2 set1

        PIPELINE_INSTANCE_TYPE pipeline_type = PIPELINE_INSTANCE_TYPE_NONE;

        std::queue<std::pair<uint32_t, VkDescriptorPool>> old_descriptor_pools;
        std::vector<VK_Buffer> uniform_buffers; //frame0 binding0 |frame0 binding1 | frame1 binding0 | frame1 binding1| frame2 binding0 | frame2 binding1
        std::vector<VkWriteDescriptorSetAccelerationStructureKHR> acceleration_structure_infos;
        std::vector<VkDescriptorImageInfo*> image_infos;
        std::vector<VkDescriptorBufferInfo*> buffer_infos;
	    std::vector<VkBufferView*> buffer_views;


	    RawVector<uint32_t> set_indices;//frame0 set0 |frame0 set1 | frame1 set0 | frame1 set1| frame2 set0 | frame2 set1
		RawVector<VkDescriptorSet> descriptor_set_handles; //frame0 set0 |frame0 set1 | frame1 set0 | frame1 set1| frame2 set0 | frame2 set1
	    RawVector<bool> dirty_descriptor_sets_bindings; //frame0 binding0 |frame0 binding1 | frame1 binding0 | frame1 binding1| frame2 binding0 | frame2 binding1
	    RawVector<VkWriteDescriptorSet> writes;

		bool bound = false;
    public:
        VK_PipelineInstance() = default;
        ~VK_PipelineInstance() = default;
        VK_PipelineInstance(const VK_PipelineInstance&) = delete;
        VK_PipelineInstance& operator=(const VK_PipelineInstance&) = delete;
        PIPELINE_INSTANCE_TYPE getType();
        void alloc(VK_Context* context, const PipelineInstanceInfo& info);
        bool allocated();
        void release();
        void onFrameEnd(uint32_t frame);
        Handle getPipeline();
        void bind();
        void getBinding(const char* name, uint32_t& uint32);
        uint32_t getBinding(const char* name) const;
        void bind(VkCommandBuffer command_buffer, uint32_t frame);
        void unbind();
        void setUniform(uint32_t binding, const void* data);
        void setImage(uint32_t binding, ImageHandle image, uint32_t mip_level);
        void setImageArray(uint32_t binding, ImageHandle* images, uint32_t image_count, int32_t mip_level);
        void setTexelBuffer(uint32_t binding, TexelBufferHandle buffer);
        void setTexelBufferArray(uint32_t binding, TexelBufferHandle* buffers, uint32_t buffer_count);
        void setAccelerationStructure(uint32_t binding, RootAccelerationStructureHandle acceleration_structure);
        void setStorageBuffer(uint32_t binding, BufferHandle buffer, size_t byte_offset);
        void setStorageBufferArray(uint32_t binding, BufferHandle* buffers, uint32_t count);

    private:
        void setBindingDirty(uint32_t index);
        void createDescriptorWrites();
        void updateDescriptors();
        uint32_t getBindingIndexForFrame(uint32_t binding);
        VkDescriptorSet getDescriptorSetForBinding(uint32_t binding);
    };
}
