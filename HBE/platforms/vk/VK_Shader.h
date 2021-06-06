#pragma once

#include "core/resource/IShader.h"
#include "vulkan/vulkan.h"
#include "string"
namespace HBE {
    class VK_Renderer;
    class VK_Device;
    class VK_Shader : public IShader {
        const VK_Device* device;
        VkShaderModule handle;
    public:

        VK_Shader(const VK_Device* device);
        ~VK_Shader();

        void setSource(const std::vector<char> &source, SHADER_TYPE type) override;
        const VkShaderModule &getHandle()const;

    };
}

