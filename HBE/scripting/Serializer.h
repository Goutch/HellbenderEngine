#pragma once
#include "string"
#include "lua.hpp"
class Serializer {
private: lua_State* L;

public:
    Serializer(std::string file_path){
        L=luaL_newstate();
    }
    void start(std::string name){};
    void end(){};
    void startArray(std::string name){};
    void endArray(){};
    template<typename T>
    void addField(std::string name,T v){

    }

};
