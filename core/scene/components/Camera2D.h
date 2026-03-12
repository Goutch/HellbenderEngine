#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/interface/RasterizationTargetInterface.h"
#include "core/resource/RasterizationTarget.h"
#include "core/scene/ecs/Component.h"

namespace HBE
{
    struct HB_API Camera2D
    {
        COMPONENT_IDS(Camera2D);
        mat4 projection = mat4(1.0f);
        RasterizationTargetHandle render_target = HBE_NULL_HANDLE;
        uint32_t layer_mask = UINT32_MAX;
        bool active = true;

    private:
        float far = 1000.0f;
        float near = 0.1f;
        float zoom_ratio = 10.0f;
        float aspect_ratio = 0.0f;
        bool projection_dirty = true;

    public:
        void setZoomRatio(float zoom);

        float getZoomRatio();

        void setNearPlane(float near);

        void setFarPlane(float far);

        float getNearPlane();

        float getFarPlane();

        void calculateAspectRatio(vec2u res);

        void calculateProjection();

        bool isProjectionDirty() const;
    };
}
