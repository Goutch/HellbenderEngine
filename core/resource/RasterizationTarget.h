#pragma once

#include "Core.h"
#include "core/interface/ImageInterface.h"
#include "core/interface/RasterizationTargetInterface.h"
#include "dependencies/utils-collection/Event.h"

#include "core/Graphics.h"

namespace HBE
{
    class HB_API RasterizationTarget
    {
        RasterizationTargetHandle handle = HBE_NULL_HANDLE;
        Context& context;

    public:
        Event<RasterizationTarget*> onResolutionChange;

        RasterizationTarget();
        explicit RasterizationTarget(const RasterizationTargetInfo& info);

        void setResolution(vec2u);
        vec2u getResolution();
        ImageHandle getFramebufferTexture(uint32_t frame_index);
    };
}
