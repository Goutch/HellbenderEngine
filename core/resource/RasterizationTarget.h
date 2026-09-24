#pragma once


#include "Core.h"
#include "core/Graphics.h"
#include "core/interface/RasterizationTargetInterface.h"
#include "dependencies/utils-collection/Event.h"


namespace HBE {
    class HB_API RasterizationTarget {
        RasterizationTargetHandle handle = HBE_NULL_HANDLE;
        ContextBase<VK_Context> &context;

    public:
		void alloc(const RasterizationTargetInfo &info);
		bool allocated();
		void release();

        Event<RasterizationTarget *> onResolutionChange;

        RasterizationTarget();
		~RasterizationTarget();

        explicit RasterizationTarget(const RasterizationTargetInfo &info);

        RasterizationTargetHandle getHandle();

        void setResolution(vec2u);

        vec2u getResolution();

        ImageHandle getFramebufferTexture();
    };
}
