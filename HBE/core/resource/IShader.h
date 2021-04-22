#pragma once
namespace HBE{
    enum class SHADER_TYPE {
        COMPUTE, VERTEX, FRAGMENT, GEOMETRY, NONE
    };
    class IShader{
    public:
        virtual ~IShader() =default;
        virtual void setSource(const std::string& source,SHADER_TYPE type)=0;
        virtual const void* getHandle() const=0;
    };
}

