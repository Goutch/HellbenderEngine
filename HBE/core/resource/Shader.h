#pragma once

#include "Core.h"
#include "string"
namespace HBE {
    enum class SHADER_TYPE {
        COMPUTE, VERTEX, FRAGMENT, GEOMETRY
    };
    class Shader {
    protected:
        SHADER_TYPE type;
    public:
        static std::string getSource(const std::string &path);
        static Shader* create(SHADER_TYPE type,const std::string& source);
        Shader(SHADER_TYPE type,const std::string& source);
        virtual ~Shader(){};
        virtual unsigned int getHandle()=0;
    };
}

