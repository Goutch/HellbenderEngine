#pragma once

#include "Core.h"
#include "string"
#include "Resource.h"
#include "map"
#include "memory"
#include "vector"

namespace HBE {
    enum class SHADER_TYPE {
        COMPUTE, VERTEX, FRAGMENT, GEOMETRY, NONE
    };

    class HB_API Shader : public Resource {

        SHADER_TYPE type;
    public:
        void load(const std::string &path, SHADER_TYPE type);

        virtual void setSource(const std::vector<char> &source, SHADER_TYPE type) = 0;

        SHADER_TYPE getType();

        virtual ~Shader() = default;

    private:
        void getSource(const std::string &path, std::vector<char> &buffer);

    };
}

