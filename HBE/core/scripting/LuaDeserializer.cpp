#include "LuaDeserializer.h"
#include <lua.hpp>
LuaDeserializer::LuaDeserializer(std::string path) {
    L = luaL_newstate();
}

LuaDeserializer::~LuaDeserializer() {
    lua_close(L);
}

void LuaDeserializer::getObject(const std::string &name) {

}

void LuaDeserializer::popObject() {

}

void LuaDeserializer::popArray() {

}

mat4 LuaDeserializer::getMat4(const std::string &name) {
    return glm::mat4();
}

vec3 LuaDeserializer::getVec3(const std::string &name) {
    return glm::vec3();
}

vec2 LuaDeserializer::getVec2(const std::string &name) {
    return glm::vec2();
}

float LuaDeserializer::getFloat(const std::string &name) {
    return 0;
}

int LuaDeserializer::getInt(const std::string &name) {
    return 0;
}

std::string LuaDeserializer::getString(const std::string &name) {
    return std::string();
}
