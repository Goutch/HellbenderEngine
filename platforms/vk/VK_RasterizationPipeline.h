#pragma once

#include <core/resource/RasterizationPipeline.h>
#include <array>
#include "vulkan/vulkan.h"
#include "unordered_map"
#include "VK_Image.h"
#include "VK_Shader.h"
#include "unordered_map"

namespace HBE {
	class VK_Device;

	class VK_RenderPass;

	class VK_Renderer;

	class VK_Buffer;

	class VK_PipelineLayout;

	class VK_RasterizationPipeline : public RasterizationPipeline {
		VkPipeline handle = VK_NULL_HANDLE;
		VK_PipelineLayout *layout = nullptr;

		VK_Device *device = nullptr;
		VK_Renderer *renderer = nullptr;
		std::vector<const VK_Shader *> shaders;
		VkRenderPass render_pass = VK_NULL_HANDLE;
		std::vector<VertexAttributeInfo> binding_infos;
		RasterizationPipelineInfo info;

		mutable bool is_bound = false;

	public:
		VK_RasterizationPipeline(VK_Device *device, VK_Renderer *renderer, const RasterizationPipelineInfo &info, VkRenderPass render_pass);
		VK_RasterizationPipeline(VK_Device *device, VK_Renderer *renderer, const RasterizationPipelineInfo &info);

		void pushConstant(const std::string &name, const void *data) const override;

		~VK_RasterizationPipeline() override;

		void bind() const override;

		void unbind() const override;

		void createPipelineLayout();

		bool bound();

		const VK_PipelineLayout *getPipelineLayout() const;

		void createRenderPass(VK_Device *device, VK_Renderer *renderer);

		RASTERIZATION_PIPELINE_FLAGS getFlags() const override;
	};
}
