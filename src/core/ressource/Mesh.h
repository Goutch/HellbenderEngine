
#pragma once
#include <vector>
#include "Ressource.h"
#include "unordered_map"
#include "glm/glm.hpp"
using namespace glm;

class Mesh : public Ressource{
protected:
    unsigned int vertex_count=0;
    unsigned int index_count=0;
    bool has_index_buffer=false;
public:
    virtual ~Mesh(){};
    virtual void setIndices(const std::vector<unsigned int>& data)=0;
    virtual void setBuffer(unsigned int position,const std::vector<float>& data)=0;
    virtual void setBuffer(unsigned int position,const std::vector<vec2>& data)=0;
    virtual void setBuffer(unsigned int position,const std::vector<vec3>& data)=0;
    virtual void setBuffer(unsigned int position,const std::vector<vec4>& data)=0;
    unsigned int getVertexCount() const {return vertex_count;}
    unsigned int getIndexCount() const {return index_count;}
    bool hasIndexBuffer() const {return has_index_buffer;}
};

