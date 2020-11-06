#pragma once
#include "string"
class Serializer {
public:
    Serializer(std::string file_path){

    }
    void start(std::string name){};
    void end(){};
    void startArray(std::string name){};
    void endArray(){};
    template<typename T>
    void addField(std::string name,T v){

    }

};
