#pragma once
#include "string"
class Serializer {
    Serializer(std::string file_path){

    }
    void start(std::string name);
    template<typename T>
    void addField(std::string name,T v){

    }
    void end();
};
