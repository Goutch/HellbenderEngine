
#include <glm/detail/type_quat.hpp>
#include "Serializer.h"

Serializer::Serializer(std::string file_path) {
    file.open(file_path);
    arrays.emplace(false);
}

void Serializer::begin(std::string name) {
    if (!arrays.top()) {
        file << indentation << name << "={" << std::endl;
    } else {
        file << indentation << "{" << std::endl;
    }
    depth++;
    indentation += '\t';
    arrays.emplace(false);
}

void Serializer::end() {
    depth--;
    indentation.pop_back();
    if (depth != 0)
        file << indentation << "}," << std::endl;
    else
        file << indentation << "}" << std::endl;
    arrays.pop();
}

void Serializer::beginArray(std::string name) {
    if (!arrays.top())
        file << indentation << name << "={" << std::endl;
    else
        file << indentation << '{' << std::endl;

    depth++;
    indentation += '\t';
    arrays.emplace(true);
}

void Serializer::endArray() {
    end();
}

void Serializer::addField(std::string name, std::string value) {
    if (!arrays.top()) {
        file << indentation << name << "=\"" << value << "\"," << std::endl;
    } else {
        file << indentation << "\"" << value << "\"," << std::endl;
    }
}

void Serializer::addField(std::string name, int value) {
    if (!arrays.top())
        file << indentation << name << '=' << std::to_string(value) << ',' << std::endl;
    else
        file << indentation << std::to_string(value) << ',' << std::endl;

}

void Serializer::addField(std::string name, float value) {
    if (!arrays.top())
        file << indentation << name << '=' << std::to_string(value) << ',' << std::endl;
    else
        file << indentation << std::to_string(value) << ',' << std::endl;

}

void Serializer::addField(std::string name, bool value) {
    if (!arrays.top())
        file << indentation << name << '=' << (value ? "true" : "false") << ',' << std::endl;
    else
        file << indentation << (value ? "true" : "false") << ',' << std::endl;

}

void Serializer::addField(std::string name, vec3 value) {
    if (!arrays.top())
        file << indentation << name << "={" << std::to_string(value.x) << ',' << std::to_string(value.y) << ',' << std::to_string(value.z) << "},"
             << std::endl;
    else
        file << indentation << '{' << std::to_string(value.x) << ',' << std::to_string(value.y) << ',' << std::to_string(value.z) << "},"
             << std::endl;
}

void Serializer::addField(std::string name, vec2 value) {
    if (!arrays.top())
        file << indentation << name << "={" << std::to_string(value.x) << ',' << std::to_string(value.y) << "}," << std::endl;
    else
        file << indentation << '{' << std::to_string(value.x) << ',' << std::to_string(value.y) << "}," << std::endl;

}

void Serializer::addField(std::string name, vec4 value) {
    if (!arrays.top())
        file << indentation << name << "={" << std::to_string(value.x) << ',' << std::to_string(value.y) << ',' << std::to_string(value.z) << ','
             << std::to_string(value.w) << "}," << std::endl;
    else
        file << indentation << '{' << std::to_string(value.x) << ',' << std::to_string(value.y) << ',' << std::to_string(value.z) << ','
             << std::to_string(value.w) << "}," << std::endl;

}

void Serializer::addField(std::string name, quat value) {
    if (!arrays.top()) {
        file << indentation << name << "={" << std::to_string(value.x) << ',' << std::to_string(value.y) << ',' << std::to_string(value.z) << ','
             << std::to_string(value.w) << "}," << std::endl;
    } else
        file << indentation << '{' << std::to_string(value.x) << ',' << std::to_string(value.y) << ',' << std::to_string(value.z) << ','
             << std::to_string(value.w) << "}," << std::endl;
}

Serializer::~Serializer() {
    file.close();
}






