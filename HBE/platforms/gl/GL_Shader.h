#pragma once

#include "Core.h"
#include "core/resource/Shader.h"
#include "string"

namespace HBE {
    class GL_Shader : public IShader {
    protected:
        unsigned int handle;
    public:
        virtual ~GL_Shader() override;

        void setSource(const std::string &source, SHADER_TYPE type) override;


    };
}

