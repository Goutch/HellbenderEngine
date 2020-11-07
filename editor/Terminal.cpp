//
// Created by User on 07-Nov.-2020.
//

#include <imgui.h>
#include "Terminal.h"
const char* Terminal::name="Terminal";
void Terminal::draw(bool& active) {

    ImGui::Begin(name,&active);


    ImGui::End();
}
