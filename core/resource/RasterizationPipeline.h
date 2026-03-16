#pragma once

#include "Core.h"
#include "core/interface/RasterizationPipelineInterface.h"
#include "core/Graphics.h"


namespace HBE {
    class PipelineInstance;

    class HB_API RasterizationPipeline {
        Context &context;
        RasterizationPipelineHandle handle = HBE_NULL_HANDLE;
    public:
        RasterizationPipeline();

        ~RasterizationPipeline();

        explicit RasterizationPipeline(const RasterizationPipelineInfo &info);

        void alloc(const RasterizationPipelineInfo &info);

        void allocInstance(PipelineInstance &instance);

        void release();

        RasterizationPipelineHandle getHandle();

        uint32_t getBinding(const char *name);

    };

}
