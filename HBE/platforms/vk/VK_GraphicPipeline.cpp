//
// Created by user on 4/17/2021.
//

#include "VK_GraphicPipeline.h"
#include "vulkan/vulkan.h"
#include "VK_Shader.h"

HBE::VK_GraphicPipeline::~VK_GraphicPipeline() {
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
}

void HBE::VK_GraphicPipeline::bind() const {

}

void HBE::VK_GraphicPipeline::unbind() const {

}

void HBE::VK_GraphicPipeline::setShaders(const std::vector<Shader *> &shaders) {
    VkPipelineShaderStageCreateInfo shaderStages[shaders.size()] ;
    for (int i = 0; i < shaders.size(); ++i) {
        VkShaderStageFlagBits stage;
        switch (shaders[i]->getType()) {
            case SHADER_TYPE::FRAGMENT:
                VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case SHADER_TYPE::VERTEX:
                VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case SHADER_TYPE::COMPUTE:
                VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
                break;
        }
        VkPipelineShaderStageCreateInfo ShaderStageInfo{};
        ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStageInfo.stage = stage;
        ShaderStageInfo.module = static_cast<VK_Shader *>(shaders[i])->getHandle();
        ShaderStageInfo.pName = "main";
        shaderStages[i]=ShaderStageInfo;
    }

}

void HBE::VK_GraphicPipeline::setUniform(std::string name, int i) const {

}

void HBE::VK_GraphicPipeline::setUniform(std::string name, float f) const {

}

void HBE::VK_GraphicPipeline::setUniform(std::string name, const glm::vec2 &v) const {

}

void HBE::VK_GraphicPipeline::setUniform(std::string name, const glm::vec3 &v) const {

}

void HBE::VK_GraphicPipeline::setUniform(std::string name, const glm::vec4 &v) const {

}

void HBE::VK_GraphicPipeline::setUniform(std::string name, const glm::mat3 &m) const {

}

void HBE::VK_GraphicPipeline::setUniform(unsigned int location, int i) const {

}

void HBE::VK_GraphicPipeline::setUniform(unsigned int location, float f) const {

}

void HBE::VK_GraphicPipeline::setUniform(unsigned int location, const glm::vec2 &v) const {

}

void HBE::VK_GraphicPipeline::setUniform(unsigned int location, const glm::vec3 &v) const {

}

void HBE::VK_GraphicPipeline::setUniform(unsigned int location, const glm::vec4 &v) const {

}

void HBE::VK_GraphicPipeline::setUniform(unsigned int location, const glm::mat3 &m) const {

}

void HBE::VK_GraphicPipeline::setUniform(unsigned int location, const glm::mat4 &m) const {

}

void HBE::VK_GraphicPipeline::setUniformIntArray(unsigned int location, int *i, unsigned int count) const {

}

void HBE::VK_GraphicPipeline::setUniformFloatArray(unsigned int location, float *f, unsigned int count) const {

}

void HBE::VK_GraphicPipeline::setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const {

}

void HBE::VK_GraphicPipeline::setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const {

}

void HBE::VK_GraphicPipeline::setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const {

}

void HBE::VK_GraphicPipeline::setUniform(std::string name, const glm::mat4 &m) const {

}
