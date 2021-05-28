
#include "VK_Shader.h"
#include "core/utility/Log.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "ShaderCompiler.h"

namespace HBE {

    VK_Shader::~VK_Shader() {
        if (handle)
            vkDestroyShaderModule(device->getHandle(), handle, nullptr);
    }

    VK_Shader::VK_Shader(const VK_Device *device) {
        this->device = device;
    }

    void VK_Shader::setSource(const std::vector<char> &source, SHADER_TYPE type) {
        std::vector<uint32_t> spirv;
        ShaderCompiler::GLSLToSpirV(source,spirv,type);
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = source.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(source.data());

        if (vkCreateShaderModule(device->getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
            Log::error("failed to create shader module!");
        }
    }

    const void *VK_Shader::getHandle() const {
        return &handle;
    }


}
