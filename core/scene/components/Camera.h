//
// Created by User on 3/22/2023.
//

#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/resource/RasterizationTarget.h"
#include "core/scene/ecs/Component.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glm/gtc/quaternion.hpp"

namespace HBE
{
    struct HB_API Camera
    {
        COMPONENT_IDS(Camera)
        mat4 projection = mat4(1.0f);
        RasterizationTargetHandle render_target = HBE_NULL_HANDLE;
        uint32_t layer_mask = UINT32_MAX;
        bool active = true;
    private:
        float fov = 70.0f;
        float far = 1000.0f;
        float near = 0.1f;
        float aspect_ratio = 0.0f;
        bool projection_dirty = true;
    public:
        void setFOV(float fov);

        float getFOV();

        void setNearPlane(float near);

        void setFarPlane(float far);

        float getNearPlane();

        float getFarPlane();

        void calculateAspectRatio(vec2u res);

        void calculateProjection();

        float getAspectRatio();

        bool isProjectionDirty();
    };
}
