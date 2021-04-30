#pragma once
#include "vector"
#include "string"
#include "core/resource/IShader.h"
namespace HBE{
    class ShaderCompiler {
        static bool initialized;
    public:
        static void GLSLToSpirV(const std::string &source, std::vector<uint32_t> buffer, SHADER_TYPE type);
    };

}
