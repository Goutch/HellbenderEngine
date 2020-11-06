
#include <glm/detail/type_quat.hpp>
#include "Serializer.h"
Serializer::Serializer(std::string file_path) {
    file.open (file_path);
}

void Serializer::start(std::string name) {
    file<<indentation<<name<<"={"<<std::endl;
    depth++;
    indentation+='\t';
}

void Serializer::end() {
    file<<"}"<<std::endl;
    depth--;
    indentation.pop_back();
}

void Serializer::startArray(std::string name) {
    start(name);
}

void Serializer::endArray() {

}

void Serializer::addField(std::string name, std::string value) {
    file<<indentation<<name<<"=\""<<value<<"\","<<std::endl;
}

void Serializer::addField(std::string name, int value) {
    file<<indentation<<name<<'='<<std::to_string(value)<<','<<std::endl;
}

void Serializer::addField(std::string name, float value) {
    file<<indentation<<name<<'='<<std::to_string(value)<<','<<std::endl;
}

void Serializer::addField(std::string name, bool value) {
    file<<indentation<<name<<'='<<(value?"true":"false")<<','<<std::endl;
}
void Serializer::addField(std::string name, vec3 value) {
    file<<indentation<<name<<"={"<<std::to_string(value.x)<<','<<std::to_string(value.y)<<','<<std::to_string(value.z)<<'}'<<std::endl;
}

void Serializer::addField(std::string name, vec2 value) {
    file<<indentation<<name<<"={"<<std::to_string(value.x)<<','<<std::to_string(value.y)<<'}'<<std::endl;
}

void Serializer::addField(std::string name, vec4 value) {
    file<<indentation<<name<<"={"<<std::to_string(value.x)<<','<<std::to_string(value.y)<<','<<std::to_string(value.z)<<','<<std::to_string(value.w)<<'}'<<std::endl;
}
void Serializer::addField(std::string name, quat value) {
    file<<indentation<<name<<"={"<<std::to_string(value.x)<<','<<std::to_string(value.y)<<','<<std::to_string(value.z)<<','<<std::to_string(value.w)<<'}'<<std::endl;
}
Serializer::~Serializer() {
    file.close();
}




