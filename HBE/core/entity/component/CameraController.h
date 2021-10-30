#pragma once

#include "Core.h"
#include "Component.h"
namespace HBE {
    class Camera;

    class HB_API CameraController : public Component {
        const float MAX_PITCH = glm::radians(90.0f);
        float current_pitch = 0.0f;
        float units_per_seconds = 10.0f;
        Camera *camera;

    public:
        ~CameraController() override;

        void onUpdate(float delta) override;

        void onAttach() override;

        std::string toString() const override;

    };

}