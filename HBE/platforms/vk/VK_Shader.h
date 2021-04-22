#pragma once

#include "core/resource/IShader.h"
#include "vulkan/vulkan.h"
#include "string"
namespace HBE {
    class VK_Renderer;
    class VK_Shader : public IShader {

        const VkDevice* device_handle;
        VkShaderModule handle;
    public:

        VK_Shader(const VkDevice& device_handle);
        ~VK_Shader();

        void setSource(const std::string &source, SHADER_TYPE type) override;
        const void *getHandle() const override;

    };
}

