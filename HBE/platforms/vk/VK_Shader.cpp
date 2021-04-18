//
// Created by user on 4/17/2021.
//

#include "VK_Shader.h"
#include "shaderc/shaderc.hpp"
#include "core/utility/Log.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
namespace HBE{
    HBE::VK_Shader::VK_Shader(const VkDevice& device_handle,SHADER_TYPE type, const std::string &source) : Shader(type) {
        shaderc_compiler_t compiler = shaderc_compiler_initialize();
        shaderc_shader_kind kind;
        switch (type) {
            case SHADER_TYPE::FRAGMENT:
                kind = shaderc_shader_kind::shaderc_glsl_fragment_shader;
                break;
            case SHADER_TYPE::VERTEX:
                kind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
                break;
            case SHADER_TYPE::COMPUTE:
                kind = shaderc_shader_kind::shaderc_glsl_compute_shader;
                break;
        }
        shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler,
                                                                       source.c_str(),
                                                                       source.size(), kind,
                                                                       "",
                                                                       "main",
                                                                       nullptr);
        const char *bytes = shaderc_result_get_bytes(result);
        size_t size = shaderc_result_get_length(result);

        if (shaderc_result_get_num_errors(result) > 0) {
            Log::error(shaderc_result_get_error_message(result));
        }
        if (shaderc_result_get_num_warnings(result) > 0) {
            Log::warning(shaderc_result_get_error_message(result));
        }


        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = size;
        createInfo.pCode = reinterpret_cast<const uint32_t *>(bytes);
        VkShaderModule shaderModule;
        this->device_handle=&device_handle;
        if (vkCreateShaderModule(device_handle, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            Log::error("failed to create shader module!");
        }
    }

    VK_Shader::~VK_Shader() {
        vkDestroyShaderModule(*device_handle,handle, nullptr);
    }

    VkShaderModule &VK_Shader::getHandle() {
        return handle;
    }

}
