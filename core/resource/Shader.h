#pragma once

#include "Core.h"
#include "string"
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

        void alloc(const ShaderInfo &info);

        void release();
    };
}
