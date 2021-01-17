#pragma once

#include "Core.h"
#include "core/resource/Shader.h"
#include "string"

namespace HBE {
    class GL_Shader : public Shader {
    protected:
        unsigned int handle;
    public:
        GL_Shader(SHADER_TYPE type,const std::string& source);

        ~GL_Shader() override;

        unsigned int getHandle() override;

        static std::string getSource(const std::string &path);
    };
}

