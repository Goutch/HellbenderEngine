#pragma once

#include "Core.h"
#include "core/Graphics.h"
#include "core/interface/RasterizationPipelineInterface.h"

namespace HBE {
    class PipelineInstance;

    class HB_API RasterizationPipeline {
        Context &context;
        RasterizationPipelineHandle handle = HBE_NULL_HANDLE;

    public:
        RasterizationPipeline();

        ~RasterizationPipeline();

        RasterizationPipeline(const RasterizationPipelineInfo &info);

        void alloc(const RasterizationPipelineInfo &info);

        void allocInstance(PipelineInstance &instance);

        void release();

        RasterizationPipelineHandle getHandle();

        uint32_t getBinding(const char *name);

        bool isCompatible(VertexAttributeInfo *attribute_infos, uint32_t attribute_info_count) const;

        RASTERIZATION_PIPELINE_FLAGS getFlags() const;
    };
}
