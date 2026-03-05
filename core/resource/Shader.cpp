#include "Shader.h"
#include "core/utility/Log.h"
#include "fstream"
#include "core/Application.h"

namespace HBE {
    Shader::Shader() : context(Application::instance->getContext()) {
        handle = HBE_NULL_HANDLE;
    }

    Shader::Shader(const ShaderInfo &info) : context(Application::instance->getContext()) {
        context->createShader(handle, info);
    }

    Shader::~Shader() {
        if (handle != HBE_NULL_HANDLE)
            context->releaseShader(handle);
    }

    void Shader::alloc(const ShaderInfo &info) {
        context->createShader(handle, info);
    }

    void Shader::release() {
        if (handle != HBE_NULL_HANDLE)
            context->releaseShader(handle);
    }

}
