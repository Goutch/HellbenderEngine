#include <core/graphics/Graphics.h>
#include "VK_GraphicPipeline.h"
#include "VK_Shader.h"
#include "core/graphics/Window.h"

#include "VK_Device.h"

#include "VK_RenderPass.h"
#include "VK_CommandPool.h"
#include "VK_Renderer.h"
#include "VK_Swapchain.h"
#include "VK_VertexLayout.h"

namespace HBE {
    VK_GraphicPipeline::VK_GraphicPipeline(VK_Device *device, VK_Renderer *renderer) {
        this->device = device;
        this->renderer = renderer;
    }

    VK_GraphicPipeline::~VK_GraphicPipeline() {
        vkDestroyPipeline(device->getHandle(), handle, nullptr);
        vkDestroyPipelineLayout(device->getHandle(), pipeline_layout_handle, nullptr);
    }


    void VK_GraphicPipeline::bind() const {
        vkCmdBindPipeline(renderer->getCommandPool()->getCurrentBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
    }

    void VK_GraphicPipeline::unbind() const {
        vkCmdBindPipeline(renderer->getCommandPool()->getCurrentBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
    }

    void VK_GraphicPipeline::setDrawFlags(DRAW_FLAGS flags) {

    }

    DRAW_FLAGS VK_GraphicPipeline::getDrawFlags() const {
        return 0;
    }

    void VK_GraphicPipeline::setShaders(const Shader *vertex, const Shader *fragment, const VertexLayout *layout) {
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = (dynamic_cast<const VK_Shader *>(vertex)->getHandle());
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = (dynamic_cast<const VK_Shader *>(fragment)->getHandle());
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[2] = {vertShaderStageInfo, fragShaderStageInfo};

        //--------------------------VertexLayout input-----------------------------------
        const VK_VertexLayout *vk_layout = (const VK_VertexLayout *) layout;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &vk_layout->getBindingDescription();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vk_layout->getAttributeDescriptions().size());
        vertexInputInfo.pVertexAttributeDescriptions = vk_layout->getAttributeDescriptions().data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        //-------------------Viewports and scissors--------------------
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        int width, height;
        Graphics::getWindow()->getSize(width, height);
        viewport.width = (float) width;
        viewport.height = (float) height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = VkExtent2D{(uint32_t) width, (uint32_t) height};

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        //---------------------------Rasterizer------------------------
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        /* VkDynamicState dynamicStates[] = {
                  VK_DYNAMIC_STATE_VIEWPORT,
                  VK_DYNAMIC_STATE_LINE_WIDTH
          };

          VkPipelineDynamicStateCreateInfo dynamicState{};
          dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
          dynamicState.dynamicStateCount = 2;
          dynamicState.pDynamicStates = dynamicStates;*/


        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(device->getHandle(), &pipelineLayoutInfo, nullptr, &pipeline_layout_handle) !=
            VK_SUCCESS) {
            Log::error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr; // Optional
        pipelineInfo.pTessellationState = VK_NULL_HANDLE;
        pipelineInfo.layout = pipeline_layout_handle;

        pipelineInfo.renderPass = renderer->getSwapchain().getRenderPass().getHandle();
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(device->getHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &handle) !=
            VK_SUCCESS) {
            Log::error("failed to create graphics pipeline!");
        }
    }

    void VK_GraphicPipeline::setShaders(const Shader *vertex, const Shader *geometry,
                                        const Shader *fragment, const VertexLayout *layout) {
        //todo:vulkan geometry shader support
        Log::error("Geometry shader not implemented in vulkan renderer");
    }

    void VK_GraphicPipeline::setUniform(unsigned int location, int i) const {

    }

    void VK_GraphicPipeline::setUniform(unsigned int location, float f) const {

    }

    void VK_GraphicPipeline::setUniform(unsigned int location, const glm::vec2 &v) const {

    }

    void VK_GraphicPipeline::setUniform(unsigned int location, const glm::vec3 &v) const {

    }

    void VK_GraphicPipeline::setUniform(unsigned int location, const glm::vec4 &v) const {

    }

    void VK_GraphicPipeline::setUniform(unsigned int location, const glm::mat3 &m) const {

    }

    void VK_GraphicPipeline::setUniform(unsigned int location, const glm::mat4 &m) const {

    }

    void VK_GraphicPipeline::setUniformIntArray(unsigned int location, int *i, unsigned int count) const {

    }

    void VK_GraphicPipeline::setUniformFloatArray(unsigned int location, float *f, unsigned int count) const {

    }

    void VK_GraphicPipeline::setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const {

    }

    void VK_GraphicPipeline::setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const {

    }

    void VK_GraphicPipeline::setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const {

    }

    void VK_GraphicPipeline::setUniform(const std::string &name, int i) const {

    }

    void VK_GraphicPipeline::setUniform(const std::string &name, float f) const {

    }

    void VK_GraphicPipeline::setUniform(const std::string &name, const vec2 &v) const {

    }

    void VK_GraphicPipeline::setUniform(const std::string &name, const vec3 &v) const {

    }

    void VK_GraphicPipeline::setUniform(const std::string &name, const vec4 &v) const {

    }

    void VK_GraphicPipeline::setUniform(const std::string &name, const mat3 &m) const {

    }

    void VK_GraphicPipeline::setUniform(const std::string &name, const mat4 &m) const {

    }


}
