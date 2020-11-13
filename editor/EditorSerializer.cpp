

#include "EditorSerializer.h"
#include "imgui.h"
void EditorSerializer::begin(const std::string &name) {
    ImGui::BeginChild(name.c_str());
}

void EditorSerializer::end() {
    ImGui::EndChild();
}

void EditorSerializer::beginArray(const std::string &name) {

}

void EditorSerializer::endArray() {

}

void EditorSerializer::addField(const std::string &name, const std::string &value) {

}

void EditorSerializer::addField(const std::string &name, int value) {

}

void EditorSerializer::addField(const std::string &name, float value) {

}

void EditorSerializer::addField(const std::string &name, bool value) {
    ImGui::RadioButton(name.c_str(),&value);
}

void EditorSerializer::addField(const std::string &name, vec3 value) {

}

void EditorSerializer::addField(const std::string &name, vec2 value) {

}

void EditorSerializer::addField(const std::string &name, vec4 value) {

}

void EditorSerializer::addField(const std::string &name, quat value) {

}
