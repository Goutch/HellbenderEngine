#include "glm/glm.hpp"
#include "core/serialization/Serializer.h"
#include <fstream>
#include "string"
#include "stack"
#include "Core.h"
using namespace glm;
class HB_API LuaSerializer :Serializer{
    int depth=0;
    std::ofstream file;
    std::string indentation;
    std::stack<bool> arrays;
public:
    LuaSerializer();
    ~LuaSerializer();
    virtual void begin(const std::string& name);
    virtual void end();
    virtual void beginArray(const std::string& name);
    virtual void endArray();

    virtual void addField(const std::string& name,const std::string& value);
    virtual void addField(const std::string& name,int value);
    virtual void addField(const std::string& name,float value);
    virtual void addField(const std::string& name,bool value);
    virtual void addField(const std::string& name,vec3 value);
    virtual void addField(const std::string& name,vec2 value);
    virtual void addField(const std::string& name,vec4 value);
    virtual void addField(const std::string& name,quat value);
};