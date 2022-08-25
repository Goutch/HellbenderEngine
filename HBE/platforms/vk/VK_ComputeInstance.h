#pragma once

#include <core/resource/ComputeInstance.h>
#include <string>

namespace HBE {
	class VK_Renderer;

	class VK_PipelineDescriptors;

	class ComputePipeline;

	class VK_Fence;

	class VK_ComputePipeline;

	class VK_ComputeInstance : public ComputeInstance {
		const VK_ComputePipeline *pipeline;
		VK_PipelineDescriptors *descriptors;
		VK_Renderer *renderer;
		VK_Fence *fence;
	public:
		VK_ComputeInstance(VK_Renderer *renderer, const ComputeInstanceInfo &info);
		~VK_ComputeInstance() override;
		void bind() const override;
		void unbind() const override;

		void setDynamicUniform(uint32_t binding, const void *data) override;
		void setUniform(uint32_t binding, const void *data) override;
		void setTexture(uint32_t binding, const Texture *texture, uint32_t mip_level) override;
		void setTexture(uint32_t binding, const RenderTarget *render_target) override;
		void setDynamicUniform(const std::string &name, const void *data) override;
		void setUniform(const std::string &name, const void *data) override;
		void setTexture(const std::string &name, const Texture *texture, uint32_t mip_level) override;
		void setTexture(const std::string &name, const RenderTarget *render_target) override;
		void dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) override;
		bool isDispatchFinished() override;
		void wait() override;

		const ComputePipeline *getComputePipeline() const override;
	};
}

