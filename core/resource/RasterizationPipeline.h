#pragma once

#include "Core.h"
#include "string"
#include "vector"
#include "glm/glm.hpp"
#include "Resource.h"
#include "core/interface/RasterizationPipelineInterface.h"

namespace HBE {


    class HB_API RasterizationPipeline {
        Context& context;
        RasterizationPipelineHandle handle;
    public:
        RasterizationPipeline(const RasterizationPipelineInfo& info);
        ~RasterizationPipeline();

        bool isCompatible(VertexAttributeInfo *attribute_infos, uint32_t attribute_info_count) const;

        RASTERIZATION_PIPELINE_FLAGS getFlags() const;
    };


}
