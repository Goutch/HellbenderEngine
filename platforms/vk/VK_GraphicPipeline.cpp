#include <core/graphics/Graphics.h>
#include <core/resource/Resources.h>
#include "VK_GraphicPipeline.h"
#include "core/graphics/Window.h"
#include "VK_PipelineLayout.h"
#include "VK_Renderer.h"
#include "VK_RenderPass.h"
#include "VK_CommandPool.h"
#include "VK_Swapchain.h"
#include "algorithm"
#include "VK_Fence.h"

namespace HBE {

	VK_GraphicPipeline::VK_GraphicPipeline(VK_Device *device, VK_Renderer *renderer, const GraphicPipelineInfo &info, VkRenderPass render_pass) {
		this->info = info;
		this->render_pass = render_pass;
		createRenderPass(device, renderer);
	}


	VK_GraphicPipeline::VK_GraphicPipeline(VK_Device *device, VK_Renderer *renderer, const GraphicPipelineInfo &info) {
		this->info = info;
		createRenderPass(device, renderer);
	}

	void VK_GraphicPipeline::createRenderPass(VK_Device *device, VK_Renderer *renderer) {
		this->device = device;
		this->renderer = renderer;
		this->binding_infos = std::vector<VertexAttributeInfo>(info.attribute_infos, info.attribute_infos + info.attribute_info_count);

		const VK_Shader *vk_vertex = (dynamic_cast<const VK_Shader *>(info.vertex_shader));
		const VK_Shader *vk_frag = (dynamic_cast<const VK_Shader *>(info.fragment_shader));
		shaders.emplace_back(vk_vertex);
		shaders.emplace_back(vk_frag);
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vk_vertex->getHandle();
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = vk_frag->getHandle();
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[2] = {vertShaderStageInfo, fragShaderStageInfo};


		//--------------------------MeshLayout input-----------------------------------

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		std::vector<VkVertexInputBindingDescription> binding_descriptions;
		binding_descriptions.resize(info.attribute_info_count);
		for (size_t i = 0; i < info.attribute_info_count; ++i) {
			binding_descriptions[i].binding = info.attribute_infos[i].location;
			binding_descriptions[i].inputRate =
					(info.attribute_infos[i].flags & VERTEX_ATTRIBUTE_FLAG_PER_INSTANCE) == VERTEX_ATTRIBUTE_FLAG_PER_INSTANCE ?
					VK_VERTEX_INPUT_RATE_INSTANCE :
					VK_VERTEX_INPUT_RATE_VERTEX;
			binding_descriptions[i].stride = info.attribute_infos[i].size;
		}
		vertexInputInfo.vertexBindingDescriptionCount = binding_descriptions.size();
		vertexInputInfo.pVertexBindingDescriptions = binding_descriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
		std::vector<VK_VertexAttributeInfo> vertex_inputs = vk_vertex->getVertexInputs();
		attribute_descriptions.resize(vertex_inputs.size());
		uint32_t offset = 0;
		uint32_t binding = 0;
		if (!binding_descriptions.empty()) {
			for (size_t i = 0; i < vertex_inputs.size(); ++i) {
				for (size_t j = 0; j < binding_descriptions.size() - 1; ++j) {
					if (binding == binding_descriptions[j].binding && offset == binding_descriptions[j].stride) {
						offset = 0;
						binding = binding_descriptions[j + 1].binding;
						break;
					}
				}
				attribute_descriptions[i].location = vertex_inputs[i].location;
				attribute_descriptions[i].format = vertex_inputs[i].format;
				attribute_descriptions[i].offset = offset;
				attribute_descriptions[i].binding = binding;
				offset += vertex_inputs[i].size;

			}
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
			vertexInputInfo.pVertexAttributeDescriptions = attribute_descriptions.data();
		}


		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

		switch (info.topology) {
			case VERTEX_TOPOLOGY_TRIANGLE:
				inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				break;
			case VERTEX_TOPOLOGY_LINE:
				inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		}
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		//-------------------Viewports and scissors--------------------

		uint32_t width, height;
		Graphics::getWindow()->getSize(width, height);
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = (float) height;
		viewport.width = (float) width;
		viewport.height = (float) -height;
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
		rasterizer.frontFace = (info.flags & GRAPHIC_PIPELINE_FLAG_FRONT_COUNTER_CLOCKWISE) ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		rasterizer.cullMode |= (info.flags & GRAPHIC_PIPELINE_FLAG_CULL_BACK) == GRAPHIC_PIPELINE_FLAG_CULL_BACK
		                       ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
		rasterizer.cullMode |= (info.flags & GRAPHIC_PIPELINE_FLAG_CULL_FRONT) == GRAPHIC_PIPELINE_FLAG_CULL_FRONT
		                       ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_NONE;
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

		VkDynamicState dynamicStates[] = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR,
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable =
				(info.flags & GRAPHIC_PIPELINE_FLAG_NO_DEPTH_TEST) == GRAPHIC_PIPELINE_FLAG_NO_DEPTH_TEST ? VK_FALSE : VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional


		createPipelineLayout();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;

		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState; // Optional
		pipelineInfo.pTessellationState = VK_NULL_HANDLE;
		pipelineInfo.layout = layout->getHandle();

		pipelineInfo.pDepthStencilState = &depthStencil;

		if (render_pass == VK_NULL_HANDLE) {
			const VK_RenderPass *render_pass = info.render_target == nullptr ?
			                                   dynamic_cast<const VK_RenderPass *>(Graphics::getDefaultRenderTarget()) :
			                                   dynamic_cast<const VK_RenderPass *>(info.render_target);
			pipelineInfo.renderPass = render_pass->getHandle();
		} else {
			pipelineInfo.renderPass = render_pass;
		}
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(device->getHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &handle) !=
		    VK_SUCCESS) {
			Log::error("failed to create graphics pipeline!");
		}
	}


	VK_GraphicPipeline::~VK_GraphicPipeline() {
		if (handle != VK_NULL_HANDLE)
			vkDestroyPipeline(device->getHandle(), handle, nullptr);
		if (layout != nullptr)
			delete layout;
	}

	void VK_GraphicPipeline::bind() const {
		if (is_bound) return;
		vkCmdBindPipeline(renderer->getCommandPool()->getCurrentBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
		is_bound = true;
	}

	void VK_GraphicPipeline::unbind() const {
		is_bound = false;
	}

	void VK_GraphicPipeline::pushConstant(const std::string &name, const void *data) const {
		layout->pushConstant(renderer->getCommandPool()->getCurrentBuffer(), name, data);
	}

	void VK_GraphicPipeline::createPipelineLayout() {
		layout = new VK_PipelineLayout(device, shaders.data(), shaders.size(), info.flags & GRAPHIC_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR);
	}

	bool VK_GraphicPipeline::bound() {
		return is_bound;
	}

	const VK_PipelineLayout *VK_GraphicPipeline::getPipelineLayout() const {
		return layout;
	}


}
