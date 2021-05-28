#pragma once
#include "vector"
#include "string"
#include "core/resource/IShader.h"
namespace HBE{
    class ShaderCompiler {
    public:
        static void GLSLToSpirV(const std::vector<char> &source, std::vector<uint32_t> &buffer, SHADER_TYPE type);
    };

}
