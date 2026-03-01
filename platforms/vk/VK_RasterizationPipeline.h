#pragma once

#include <core/resource/RasterizationPipeline.h>
#include <array>
#include "vulkan/vulkan.h"
#include "unordered_map"
#include "VK_Image.h"
#include "VK_Shader.h"
#include "unordered_map"
#include "VK_PipelineLayout.h"

namespace HBE
{
    class VK_Device;

    class VK_RenderPass;

    class VK_Renderer;

    class VK_Buffer;

    class VK_PipelineLayout;

    class VK_RasterizationPipeline : public RasterizationPipeline
    {
        VkPipeline handle = VK_NULL_HANDLE;
        VK_PipelineLayout layout;
        VK_Context* context = nullptr;
        std::vector<const VK_Shader*> shaders;
        std::vector<VertexAttributeInfo> binding_infos;
        RasterizationPipelineInfo info;

        mutable bool is_bound = false;

    public:
        VK_RasterizationPipeline() = default;
        ~VK_RasterizationPipeline() = default;
        VK_RasterizationPipeline(VK_RasterizationPipeline&) = delete;
        VK_RasterizationPipeline& operator=(VK_RasterizationPipeline&) = delete;
        void init(VK_Context* context,const RasterizationPipelineInfo& info);
        void release();
        void pushConstant(const std::string& name, const void* data) const override;

        void bind() const override;

        void unbind() const override;

        void createPipelineLayout();

        bool bound();

        const VK_PipelineLayout& getPipelineLayout() const;


        RASTERIZATION_PIPELINE_FLAGS getFlags() const override;
    };
}
