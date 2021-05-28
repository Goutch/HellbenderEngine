#pragma once

#include "Core.h"
#include "string"
#include "Resource.h"
#include "map"
#include "IShader.h"
#include "memory"

namespace HBE {
    class HB_API Shader : public Resource {
        IShader *instance;
        SHADER_TYPE type;
    public:
        Shader(const std::vector<char> &source, SHADER_TYPE type);

        Shader();

        void load(const std::string &path, SHADER_TYPE type);

        void setSource(const std::vector<char> &source, SHADER_TYPE type);
        const void* getHandle() const;
        SHADER_TYPE getType();

        ~Shader();

    private:
        void getSource(const std::string &path, std::vector<char> &buffer);

    };
}

