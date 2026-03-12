//
// Created by User on 3/22/2023.
//

#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/resource/RasterizationTarget.h"
#include "core/scene/ecs/Component.h"

namespace HBE
{
    struct HB_API PixelCamera
    {
        COMPONENT_IDS(PixelCamera);

        mat4 projection = mat4(1.0f);
        RasterizationTargetHandle render_target = HBE_NULL_HANDLE;
        uint32_t layer_mask = UINT32_MAX;
        bool active = true;

    private:
        vec2u resolution = vec2u(1920,1080);
        float far = 1000.0f;
        float near = 0.1f;

        bool projection_dirty = true;

    public:
        void setResolution(vec2u resolution);
        vec2u getResolution();
        void setNearPlane(float near);

        void setFarPlane(float far);

        float getNearPlane();

        float getFarPlane();

        void calculateProjection();

        bool isProjectionDirty() const;
    };
}
