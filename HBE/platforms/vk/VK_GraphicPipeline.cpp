#include <core/graphics/Graphics.h>
#include <core/resource/Resources.h>
#include "VK_GraphicPipeline.h"
#include "core/graphics/Window.h"

#include "VK_Renderer.h"
#include "VK_RenderPass.h"
#include "VK_CommandPool.h"
#include "VK_Swapchain.h"
#include "VK_VertexLayout.h"
#include "algorithm"

namespace HBE {
	VK_GraphicPipeline::VK_GraphicPipeline(VK_Device *device, VK_Renderer *renderer) {
		this->device = device;
		this->renderer = renderer;
	}

	VK_GraphicPipeline::~VK_GraphicPipeline() {
		for (auto buffer_vector:uniform_buffers) {
			for (size_t i = 0; i < buffer_vector.second.size(); ++i) {
				delete buffer_vector.second[i];
			}
		}
		if(descriptor_set_layout_handle!=VK_NULL_HANDLE)vkDestroyDescriptorSetLayout(device->getHandle(), descriptor_set_layout_handle, nullptr);
		if(descriptor_pool_handle != VK_NULL_HANDLE)vkDestroyDescriptorPool(device->getHandle(), descriptor_pool_handle, nullptr);
		if(handle!=VK_NULL_HANDLE)vkDestroyPipeline(device->getHandle(), handle, nullptr);
		if(pipeline_layout_handle!=VK_NULL_HANDLE)vkDestroyPipelineLayout(device->getHandle(), pipeline_layout_handle, nullptr);
	}


	void VK_GraphicPipeline::bind() const {
		vkCmdBindPipeline(renderer->getCommandPool()->getCurrentBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
		vkCmdBindDescriptorSets(renderer->getCommandPool()->getCurrentBuffer(),
								VK_PIPELINE_BIND_POINT_GRAPHICS,
								pipeline_layout_handle,
								0,
								1,
								&descriptor_set_handles[renderer->getCurrentFrame()],
								0,
								nullptr);
	}

	void VK_GraphicPipeline::unbind() const {
		vkCmdBindPipeline(renderer->getCommandPool()->getCurrentBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
	}

	void VK_GraphicPipeline::setDrawFlags(DRAW_FLAGS flags) {
		//todo:
	}

	DRAW_FLAGS VK_GraphicPipeline::getDrawFlags() const {
		return 0;
	}

	void VK_GraphicPipeline::setShaders(const Shader *vertex, const Shader *fragment, const VertexLayout *layout) {
		if(descriptor_set_layout_handle!=VK_NULL_HANDLE)vkDestroyDescriptorSetLayout(device->getHandle(), descriptor_set_layout_handle, nullptr);
		if(descriptor_pool_handle!=VK_NULL_HANDLE)vkDestroyDescriptorPool(device->getHandle(), descriptor_pool_handle, nullptr);
		if(handle!=VK_NULL_HANDLE)vkDestroyPipeline(device->getHandle(), handle, nullptr);
		if(pipeline_layout_handle!=VK_NULL_HANDLE)vkDestroyPipelineLayout(device->getHandle(), pipeline_layout_handle, nullptr);
		descriptor_set_layout_bindings.clear();
		push_constants_ranges.clear();
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			descriptor_sets_writes[i].clear();
		}

		inputs.clear();
		name_input_index.clear();
		binding_input_index.clear();
		name_push_constant_index.clear();
		for (auto buffer_vector:uniform_buffers) {
			for (size_t i = 0; i < buffer_vector.second.size(); ++i) {
				delete buffer_vector.second[i];
			}
		}
		uniform_buffers.clear();


		const VK_Shader *vk_vertex = (dynamic_cast<const VK_Shader *>(vertex));
		const VK_Shader *vk_frag = (dynamic_cast<const VK_Shader *>(fragment));
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

		int width, height;
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
		rasterizer.cullMode = VK_CULL_MODE_NONE;
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

		VkDynamicState dynamicStates[] = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;


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
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState; // Optional
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

	void VK_GraphicPipeline::setDynamicUniform(const std::string &name, void *data) {
		//todo
	}

	void VK_GraphicPipeline::setDynamicUniform(uint32_t binding, void *data) {
		//todo
	}

	void VK_GraphicPipeline::setUniform(const std::string &name, void *data) {
		auto it = name_input_index.find(name);
		if (it != name_input_index.end()) {
			if (inputs[it->second].type == VK_Shader::UNIFORM_BUFFER) {
				setUniform(inputs[it->second].binding, data);

			} else {
				Log::error(name + " is not a uniform buffer");
			}
		} else {
			Log::error("No shader input is named:" + name);
		}
	}

	void VK_GraphicPipeline::setUniform(uint32_t binding, void *data) {
		for (VK_Buffer *buffer:uniform_buffers[binding]) {
			buffer->update(data);
		}
	}

	void VK_GraphicPipeline::pushConstant(const std::string &name, void *data) {
		auto it = name_input_index.find(name);
		if (it != name_input_index.end()) {
			if (inputs[it->second].type == VK_Shader::PUSH_CONSTANT) {
				vkCmdPushConstants(renderer->getCommandPool()->getCurrentBuffer(), pipeline_layout_handle, inputs[it->second].stage,  inputs[it->second].offset,  inputs[it->second].size, data);
			} else {
				Log::error(name + " is not a push constant");
			}
		} else {
			Log::error("No shader input is named:" + name);
		}
	}

	void VK_GraphicPipeline::setTexture(uint32_t binding, const Texture *texture) {
		//todo undefined behavior when texture data is not set;
		//use texture event
		VK_Image *vk_texture = (VK_Image *) texture;


		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorImageInfo image_info;
			image_info.imageView = vk_texture->getImageView();
			image_info.sampler = vk_texture->getSampler();
			image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptor_sets_writes[i][binding].pImageInfo = &image_info;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_sets_writes[i][binding], 0, nullptr);
		}

	}

	void VK_GraphicPipeline::setTexture(const std::string &name, const Texture *texture) {
		auto it = name_input_index.find(name);
		if (it != name_input_index.end()) {
			if (inputs[it->second].type == VK_Shader::TEXTURE_SAMPLER) {
				setTexture(inputs[it->second].binding, texture);
			} else {
				Log::error(name + " is not a texture binding");
			}
		} else {
			Log::error("No shader input is named:" + name);
		}
	}

	void VK_GraphicPipeline::createPipelineLayout() {
		for (int i = 0; i < shaders.size(); ++i) {
			auto shader_bindings = shaders[i]->getLayoutBindings();
			descriptor_set_layout_bindings.insert(descriptor_set_layout_bindings.end(), shader_bindings.begin(), shader_bindings.end());

			auto shader_ranges = shaders[i]->getPushConstantRanges();
			push_constants_ranges.insert(push_constants_ranges.end(), shader_ranges.begin(), shader_ranges.end());

			auto shader_inputs = shaders[i]->getInputs();

			for (int j = 0; j < shader_inputs.size(); ++j) {
				name_input_index.emplace(shader_inputs[j].name, inputs.size() + j);
				binding_input_index.emplace(shader_inputs[j].binding, inputs.size() + j);
			}
			inputs.insert(inputs.end(), shader_inputs.begin(), shader_inputs.end());
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = descriptor_set_layout_bindings.size();
		layoutInfo.pBindings = descriptor_set_layout_bindings.data();

		vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &descriptor_set_layout_handle);


		for (size_t i = 0; i < descriptor_set_layout_bindings.size(); ++i) {
			if (descriptor_set_layout_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; ++j) {
					uniform_buffers.emplace(descriptor_set_layout_bindings[i].binding, std::vector<VK_Buffer *>());
					auto it = binding_input_index.find(descriptor_set_layout_bindings[i].binding);
					if (it != binding_input_index.end()) {
						uniform_buffers[descriptor_set_layout_bindings[i].binding].emplace_back(new VK_Buffer(device, inputs[it->second].size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, ALLOC_FLAGS::MAPPABLE));
					} else {
						Log::error("Could not find input at binding " + std::to_string(descriptor_set_layout_bindings[i].binding));
					}
				}
		}

		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		size_t combined_image_sampler_count = 0;
		size_t uniform_buffer_count = 0;
		for (auto layout_binding:descriptor_set_layout_bindings) {
			switch (layout_binding.descriptorType) {
				case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					uniform_buffer_count++;
					break;
				case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
					combined_image_sampler_count++;
					break;
			}
		}
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = uniform_buffer_count * MAX_FRAMES_IN_FLIGHT;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = combined_image_sampler_count * MAX_FRAMES_IN_FLIGHT;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();

		poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

		if (vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &descriptor_pool_handle) != VK_SUCCESS) {
			Log::error("failed to create descriptor pool!");
		}
		createDescriptorSets();


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptor_set_layout_handle;
		pipelineLayoutInfo.pushConstantRangeCount = push_constants_ranges.size();
		pipelineLayoutInfo.pPushConstantRanges = push_constants_ranges.data();

		if (vkCreatePipelineLayout(device->getHandle(), &pipelineLayoutInfo, nullptr, &pipeline_layout_handle) !=
			VK_SUCCESS) {
			Log::error("failed to create pipeline layout!");
		}
	}
	bool comparator (VkDescriptorSetLayoutBinding i,VkDescriptorSetLayoutBinding j) { return (i.binding<j.binding); }
	void VK_GraphicPipeline::createDescriptorSets() {

		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptor_set_layout_handle);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptor_pool_handle;
		allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
		allocInfo.pSetLayouts = layouts.data();
		std::sort(descriptor_set_layout_bindings.begin(),descriptor_set_layout_bindings.end(), comparator);
		if (vkAllocateDescriptorSets(device->getHandle(), &allocInfo, descriptor_set_handles.data()) != VK_SUCCESS) {
			Log::error("failed to allocate descriptor sets!");
		}
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

			std::vector<VkWriteDescriptorSet> writes;
			std::vector<VkDescriptorBufferInfo> buffers_info;
			std::vector<VkDescriptorImageInfo> images_info;
			for (size_t j = 0; j < descriptor_set_layout_bindings.size(); ++j) {
				writes.emplace_back();
				buffers_info.emplace_back();
				images_info.emplace_back();
				writes[j].pBufferInfo = nullptr;
				writes[j].pImageInfo = nullptr;
				if (descriptor_set_layout_bindings[j].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
					buffers_info[j].buffer = uniform_buffers[j][i]->getHandle();
					buffers_info[j].offset = 0;
					auto it = binding_input_index.find(descriptor_set_layout_bindings[j].binding);
					if (it != binding_input_index.end()) {
						buffers_info[j].range = (VkDeviceSize) inputs[it->second].size;
						writes[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						writes[j].pBufferInfo = &buffers_info[j];
					} else {
						Log::error("Could not find input at binding " + std::to_string(descriptor_set_layout_bindings[j].binding));
					}

				} else if (descriptor_set_layout_bindings[j].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
					writes[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					VK_Image *default_texture = (VK_Image *) Resources::get<Texture>("DEFAULT");
					images_info[j].imageView = default_texture->getImageView();
					images_info[j].sampler = default_texture->getSampler();
					images_info[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					writes[j].pImageInfo = &images_info[j];
				} else {
					Log::error("Descriptor set type not supported yet");
				}

				writes[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writes[j].dstSet = descriptor_set_handles[i];
				writes[j].dstBinding = descriptor_set_layout_bindings[j].binding;
				writes[j].dstArrayElement = 0;

				writes[j].descriptorCount = 1;//for arrays
				writes[j].pTexelBufferView = nullptr; // Optional
				descriptor_sets_writes[i].emplace_back(writes[j]);
				vkUpdateDescriptorSets(device->getHandle(), 1, &writes[j], 0, nullptr);
			}
		}
	}
}
