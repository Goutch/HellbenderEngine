#pragma once

#include "Core.h"
#include "string"
#include "Resource.h"
#include "map"
#include "memory"
#include "vector"
#include "GLSL_TYPE.h"
namespace HBE {
    enum class SHADER_STAGE {
        COMPUTE, VERTEX, FRAGMENT, GEOMETRY
    };

    class HB_API Shader : public Resource {
	protected:
        SHADER_STAGE type;
    public:
        void load(const std::string &path, SHADER_STAGE type);

        virtual void setSource(const std::string &source, SHADER_STAGE type) = 0;

        SHADER_STAGE getType();

        virtual ~Shader() = default;


    private:
        void getSource(const std::string &path, std::vector<char> &buffer);

    };
}

