#pragma once

#include <core/resource/GraphicPipeline.h>
#include <core/resource/VertexLayout.h>
#include <array>
#include "vulkan/vulkan.h"
#include "unordered_map"
#include "VK_Image.h"
#include "VK_CONSTANTS.h"
#include "VK_Shader.h"
#include "unordered_map"

namespace HBE {
	class VK_Device;

	class VK_RenderPass;

	class VK_Renderer;

	class VK_VertexLayout;

	class VK_Buffer;



    class VK_GraphicPipeline : public GraphicPipeline {
		VK_Device *device = nullptr;
		VK_Renderer *renderer = nullptr;

		VkPipelineLayout pipeline_layout_handle = VK_NULL_HANDLE;
		VkPipeline handle = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptor_set_layout_handle= VK_NULL_HANDLE;
		VkDescriptorPool descriptor_pool_handle = VK_NULL_HANDLE;

		std::vector<const VK_Shader *> shaders;

		std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptor_set_handles;
		std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings;
		std::vector<VkPushConstantRange> push_constants_ranges;
		std::array<std::unordered_map<uint32_t ,VkWriteDescriptorSet>, MAX_FRAMES_IN_FLIGHT> descriptor_sets_writes;
		std::unordered_map<uint32_t, std::vector<VK_Buffer *>> uniform_buffers;

		std::vector<VK_Shader::ShaderInput> inputs;
		std::unordered_map<std::string,uint32_t> name_input_index;
		std::unordered_map<uint32_t ,uint32_t> binding_input_index;
		std::unordered_map<std::string,uint32_t> name_push_constant_index;
	public:
		VK_GraphicPipeline(VK_Device *device, VK_Renderer *renderer,const GraphicPipelineInfo& info);
		VK_GraphicPipeline(VK_Device *device, VK_Renderer *renderer,const GraphicPipelineInfo& info,VkRenderPass& render_pass);
		void setDynamicUniform(const std::string &name, void *data);
		void setDynamicUniform(uint32_t binding, void *data) ;
		void setUniform(const std::string &name, void *data);
		void setUniform(uint32_t binding, void *data);
		void pushConstant(const std::string &name, void *data);
		void setTexture(uint32_t binding, const Texture *texture);
		void setTexture(const std::string &name, const Texture *texture);

		~VK_GraphicPipeline() override;

		void bind() const override;

		void unbind() const override;

		void createPipelineLayout();
		void createDescriptorSets();

        void setTexture(uint32_t binding, const RenderTarget *render_target) override;

        void setTexture(const std::string &name, const RenderTarget *render_target) override;
    };
}
