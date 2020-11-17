#pragma once
#include "Core.h"
#include "Component.h"

class Camera;
class HB_API CameraController: public Component {
    const float MAX_PITCH= glm::radians(90.0f);
    Camera* camera;
    float current_pitch=0.0f;
    float units_per_seconds=10.0f;

    vec2 direction=vec2(1);
    double last_x=0,last_y=0;
    void onUpdate(float delta) override;
    void onAttach() override;
    void onDetach() override;
    void serialize(Serializer* serializer) const override;
};

