#pragma once
#include <fstream>
#include "string"
#include "stack"
#include "glm/glm.hpp"
#include "Core.h"
using namespace glm;
class HB_API Serializer {
    int depth=0;
    std::ofstream file;
    std::string indentation;
    std::stack<bool> arrays;
public:
    Serializer(std::string file_path);
    ~Serializer();
    void begin(std::string name);
    void end();
    void beginArray(std::string name);
    void endArray();

    void addField(std::string name,std::string value);
    void addField(std::string name,int value);
    void addField(std::string name,float value);
    void addField(std::string name,bool value);
    void addField(std::string name,vec3 value);
    void addField(std::string name,vec2 value);
    void addField(std::string name,vec4 value);
    void addField(std::string name,quat value);

};
