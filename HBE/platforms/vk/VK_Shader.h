#pragma once

#include "core/resource/Shader.h"
#include "vulkan/vulkan.h"
namespace HBE {
    class VK_Renderer;
    class VK_Shader : public Shader {

        const VkDevice* device_handle;
        VkShaderModule handle;
    public:
        VK_Shader(const VkDevice& device_handle, SHADER_TYPE type, const std::string &source);
        ~VK_Shader();
        VkShaderModule& getHandle();
    };
}

