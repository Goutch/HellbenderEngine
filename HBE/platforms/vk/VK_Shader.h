#pragma once

#include "core/resource/Shader.h"
#include "vulkan/vulkan.h"
#include "string"
#include "VK_Buffer.h"
#include "VK_CONSTANTS.h"

namespace HBE {
    class VK_Renderer;

    class VK_Device;

    class VK_CommandPool;

    struct VertexInputInfo {
        uint32_t location;
        uint32_t size;
        VkFormat format;
    };

    struct UniformInfo {
    	std::string name;
		VkDeviceSize size=0;
		VkDescriptorSetLayoutBinding layout_binding;

    };

    struct PushConstantInfo{
		std::string name;
		VkPushConstantRange push_constant_range;
    };

    class VK_Shader : public Shader {
    private:
        const VK_Device *device;
        VkShaderModule handle = VK_NULL_HANDLE;
        std::vector<VertexInputInfo> vertex_inputs;
        std::vector<UniformInfo> uniforms;
        std::vector<PushConstantInfo> push_constants;

        SHADER_STAGE stage;
        VkShaderStageFlagBits vk_stage;

    public:

        VK_Shader(const VK_Device *device, const ShaderInfo &info);

        ~VK_Shader();

        const VkShaderModule &getHandle() const;

        const std::vector<PushConstantInfo> &getPushConstants() const;

        const std::vector<UniformInfo> &getUniforms() const;

        const std::vector<VertexInputInfo> &getVertexInputs() const;

        SHADER_STAGE getStage() const override;
		VkShaderStageFlagBits getVkStage() const;

    private:
        void load(const std::string &path);

        void setSource(const std::vector<uint32_t> &spirv);

        void reflect(const std::vector<uint32_t> &spirv);
    };
}

