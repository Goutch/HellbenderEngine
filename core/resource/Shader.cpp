#include "Shader.h"
#include "core/utility/Log.h"
#include "fstream"
#include "core/Application.h"
#include "../utility/ShaderCompiler.h"

namespace HBE {
    Shader::Shader() : context(Application::instance->getContext()) {
        handle = HBE_NULL_HANDLE;
    }

    Shader::Shader(const ShaderInfo &info) : context(Application::instance->getContext()) {
        alloc(info);
    }

    Shader::~Shader() {
        release();
    }

    void Shader::loadGLSL(const char *path, SHADER_STAGE stage) {
        std::string source;
        ShaderCompiler::getSource(path, source);
        std::vector<uint32_t> spirv;
        ShaderCompiler::GLSLToSpirV(source.c_str(), source.size(), spirv, stage, path);

        ShaderInfo info;
        info.stage = stage;
        info.spirv = spirv.data();
        info.spirvLength = spirv.size();

        alloc(info);
    }

    void Shader::alloc(const ShaderInfo &info) {
        if (handle != HBE_NULL_HANDLE)
            Log::error("trying to allocate a shader that has already been allocated!");
        context->createShader(handle, info);
    }

    void Shader::release() {
        if (handle != HBE_NULL_HANDLE)
            context->releaseShader(handle);
        handle = HBE_NULL_HANDLE;
    }
}
