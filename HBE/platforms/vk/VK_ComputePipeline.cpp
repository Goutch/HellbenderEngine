
#include "VK_ComputePipeline.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "VK_Shader.h"
#include "VK_PipelineLayout.h"
#include "VK_CommandPool.h"

namespace HBE {
    VK_ComputePipeline::VK_ComputePipeline(VK_Renderer *renderer, ComputePipelineInfo &info) {
        this->renderer = renderer;
        const VK_Shader *vk_shader = dynamic_cast<VK_Shader *>(info.compute_shader);

        layout = new VK_PipelineLayout(renderer->getDevice(), &vk_shader, 1);

        VkPipelineShaderStageCreateInfo stage{};
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stage.module = vk_shader->getHandle();

        VkComputePipelineCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        create_info.layout = layout->getHandle();
        create_info.stage = stage;

        if (vkCreateComputePipelines(renderer->getDevice()->getHandle(), VK_NULL_HANDLE, 1, &create_info, nullptr,
                                     &handle) != VK_SUCCESS) {
            Log::error("Failed to create compute pipeline");
        }
    }

    void VK_ComputePipeline::dispatch(uint32_t group_size_x,
                                      uint32_t group_size_y,
                                      uint32_t group_size_z,
                                      uint32_t group_count_x,
                                      uint32_t group_count_y,
                                      uint32_t group_count_z) {
        VK_Queue *queue = renderer->getDevice()->getQueue(QUEUE_FAMILY_COMPUTE);
        queue->beginCommand();
        vkCmdBindPipeline(renderer->getCommandPool()->getCurrentBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, handle);
        layout->bindDescriptors(renderer->getCommandPool()->getCurrentBuffer(), renderer->getCurrentFrame());
        vkCmdDispatch(queue->getCommandPool()->getCurrentBuffer(), group_count_x, group_count_y, group_count_z);
        queue->endCommand();
        queue->submit();
    }

    void VK_ComputePipeline::wait() {
        VK_Queue *queue = renderer->getDevice()->getQueue(QUEUE_FAMILY_COMPUTE);
        queue->wait();
    }
}

