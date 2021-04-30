//
// Created by user on 4/17/2021.
//

#include "VK_Shader.h"
#include "core/utility/Log.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "ShaderCompiler.h"
namespace HBE {


    VK_Shader::~VK_Shader() {
        vkDestroyShaderModule(device->getHandle(), handle, nullptr);
    }
    VK_Shader::VK_Shader(const VK_Device *device) {
        this->device=device;
    }

    void VK_Shader::setSource(const std::string &source, SHADER_TYPE type) {

        std::vector<glm::uint32_t> spirv;
        ShaderCompiler::GLSLToSpirV(source, spirv, type);
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = spirv.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(spirv.data());
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device->getHandle(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            Log::error("failed to create shader module!");
        }
    }

    const void *VK_Shader::getHandle() const {
        return &handle;
    }



}
