
#include "Inspector.h"
#include "SceneHierarchy.h"
#include "imgui.h"
#include "HBE.h"
#include <cmath>

const char *Inspector::name = "Inspector";
std::list<Component *> Inspector::components_to_detach;

bool beginDrawComponent(Component *component) {

    ImGui::TreePush();
    if (ImGui::CollapsingHeader(component->toString().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup(("Detach_" + component->toString()).c_str());
        }
        if (ImGui::BeginPopup(("Detach_" + component->toString()).c_str())) {
            if (ImGui::Selectable("Detach"))
                Inspector::components_to_detach.push_back(component);
            ImGui::EndPopup();
        }
        ImGui::TreePush();

        return true;
    }
    ImGui::TreePop();
    return false;

}

void endDrawComponent() {
    ImGui::TreePop();
    ImGui::TreePop();
}

void drawTransform(Entity *e) {
    std::string name = std::string("Transform");
    Transform *transform = e->transform;
    vec3 position = transform->getPosition();
    ImGui::Text("Position");
    if (ImGui::DragFloat3("##position", &position[0])) {
        transform->setPosition(position);
    }
    quat rotation = e->transform->getRotation();
    vec3 eulerRot = glm::eulerAngles(rotation);
    ImGui::Text("Rotation");
    if (ImGui::DragFloat3("##rotation", &eulerRot[0], M_PI / 3600)) {
        transform->rotate(quat(eulerRot));
    }

    vec3 scale = transform->getScale();
    ImGui::Text("Scale");
    if (ImGui::DragFloat3("##scale", &scale[0], 0.1f)) {
        if (abs(scale.x) > 0.01f &&
            abs(scale.y) > 0.01f &&
            abs(scale.z) > 0.01f)
            transform->setScale(scale);
    }
}

void drawCameraComponent(Entity *e) {
    Camera *camera = e->getComponent<Camera>();

    RenderMode render_mode = camera->getRenderMode();
    ImGui::Text("Mode:");
    if (ImGui::BeginCombo("##render_mode", render_mode == PERSPECTIVE ? "Perspective" : "Orthographic", ImGuiComboFlags_NoArrowButton)) {
        bool is_selected = (render_mode == PERSPECTIVE);
        if (ImGui::Selectable("Perspective", is_selected))
            camera->setRenderMode(RenderMode::PERSPECTIVE);
        if (is_selected)
            ImGui::SetItemDefaultFocus();

        is_selected = (render_mode == ORTHOGRAPHIC); // You can store your selection however you want, outside or inside your objects
        if (ImGui::Selectable("Orthographic", is_selected))
            camera->setRenderMode(RenderMode::ORTHOGRAPHIC);
        if (is_selected)
            ImGui::SetItemDefaultFocus();

        ImGui::EndCombo();
    }

    float render_dist = camera->getRenderDistance();
    ImGui::Text("Render distance:");
    if (ImGui::InputFloat("##render_distance", &render_dist)) {
        camera->setRenderDistance(render_dist);
    }

    if (render_mode == RenderMode::PERSPECTIVE) {
        float fov = camera->getFOV();
        ImGui::Text("Field of view:");
        if (ImGui::SliderFloat("##fov", &fov, 40.0f, 170.0f)) {
            camera->setFOV(fov);
        }
    } else {
        ImGui::Text("Zoom:");
        float zoom = camera->getOrthographicZoom();
        if (ImGui::InputFloat("##zoom", &zoom)) {
            camera->setOrthographicZoom(zoom);
        }
    }
}

void Inspector::draw(bool active) {
    if (active) {
        ImGui::Begin(name, &active, ImGuiWindowFlags_NoCollapse);

        if (!SceneHierarchy::selected_entities.empty()) {
            for (Entity *e:SceneHierarchy::selected_entities) {
                ImGui::BeginGroup();
                if (ImGui::CollapsingHeader(e->getName().c_str(),
                                            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed)) {
                    for (Component *c:e->getComponents()) {
                        std::string name = c->toString();
                        if (beginDrawComponent(c)) {
                            if (name == "Transform") {
                                drawTransform(e);
                            }
                            if (name == "Camera") {
                                drawCameraComponent(e);
                            } else if (name == "MeshRenderer") {

                            } else if (name == "ModelRenderer") {

                            }
                            endDrawComponent();
                        }
                    }
                    ImGui::TreePush();
                    if (ImGui::Button("Attach Component")) {
                        ImGui::OpenPopup("Attach");
                    }
                    if (ImGui::BeginPopup("Attach")) {
                        std::list<std::string> names = ComponentRegistry::getComponentsName();
                        for (const std::string &name:names) {
                            if (ImGui::Selectable(name.c_str()))
                                ComponentRegistry::attach(name, e);

                        }
                        ImGui::EndPopup();
                    }

                    ImGui::TreePop();
                }
            }
            ImGui::EndGroup();
            ImGuiStyle &style = ImGui::GetStyle();
            float padding_x = style.WindowPadding.x / 2;

            ImGui::GetWindowDrawList()->AddRect(ImVec2(ImGui::GetItemRectMin().x - padding_x, ImGui::GetItemRectMin().y - 1),
                                                ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x + padding_x,
                                                       ImGui::GetItemRectMax().y + 5),
                                                ImColor(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]),
                                                ImGui::GetStyle().FrameRounding,
                                                ImDrawCornerFlags_All,
                                                1.f);
        }
        ImGui::End();
        for (Component *c :components_to_detach) {
            c->entity->detach(c);
        }
        components_to_detach.clear();
    }
}


