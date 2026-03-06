#pragma once

#include "Core.h"
#include "Resource.h"
#include "core/interface/ShaderInterface.h"

namespace HBE {
    class HB_API Shader {
        ShaderHandle handle;
        Context *context = nullptr;

    public:
        Shader();

        explicit Shader(const ShaderInfo &info);

        ~Shader();

        void loadGLSL(const char *path, SHADER_STAGE stage);

        void alloc(const ShaderInfo &info);

        ShaderHandle getHandle();

        void release();
    };
}
