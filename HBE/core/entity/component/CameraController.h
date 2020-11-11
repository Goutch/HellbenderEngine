#pragma once

#include "Component.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Core.h"
class Camera;
class HB_API CameraController: public Component {
    const float MAX_PITCH= M_PI / 2;
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

