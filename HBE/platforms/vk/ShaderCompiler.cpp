
#include "ShaderCompiler.h"

using namespace glslang;
namespace HBE {
    bool ShaderCompiler::initialized = false;

    void ShaderCompiler::GLSLToSpirV(const std::string &source, std::vector<uint32_t> buffer, SHADER_TYPE type) {
      /*  if (!initialized) {
            glslang::InitializeProcess();
            initialized = true;
        }
        EShLanguage stage = EShLanguage::EShLangCount;
        switch (type) {
            case SHADER_TYPE::VERTEX:
                stage = EShLangVertex;
                break;
            case SHADER_TYPE::FRAGMENT:
                stage = EShLangFragment;
                break;
            case SHADER_TYPE::GEOMETRY:
                stage = EShLangGeometry;
                break;
            case SHADER_TYPE::COMPUTE:
                stage = EShLangCompute;
                break;
        }

        glslang::TShader Shader(stage);

        const char *code = source.c_str();
        Shader.setStrings(&code, 1);

        int ClientInputSemanticsVersion = 100;

        Shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, ClientInputSemanticsVersion);
        Shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
        Shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);


        EShMessages messages = (EShMessages) (EShMsgSpvRules | EShMsgVulkanRules);
        const int DefaultVersion = 450;
        TBuiltInResource resource = {};
        glslang::TShader::ForbidIncluder includer;

        std::string PreprocessedGLSL;

        if (!Shader.preprocess(&resource, DefaultVersion, ENoProfile, false, false, messages, &PreprocessedGLSL,
                               includer)) {
            throw std::runtime_error("GLSL Preprocessing Failed");
        }

        const char *PreprocessedCStr = PreprocessedGLSL.c_str();
        Shader.setStrings(&PreprocessedCStr, 1);

        if (!Shader.parse(&resource, 100, false, messages)) {
            throw std::runtime_error("GLSL Parsing Failed");
        }

        glslang::TProgram Program;
        Program.addShader(&Shader);

        if (!Program.link(messages)) {
            throw std::runtime_error("GLSL Linking Failed");
        }
        if (!Program.mapIO()) {
            throw std::runtime_error("GLSL Linking Failed");
        }

        spv::SpvBuildLogger logger;
        glslang::SpvOptions spvOptions;
        glslang::GlslangToSpv(*Program.getIntermediate(stage), buffer, &logger, &spvOptions);*/
    }
}

