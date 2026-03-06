#include "RasterizationPipeline.h"

#include "core/Application.h"
#include "core/Context.h"
namespace HBE {
    RasterizationPipeline::RasterizationPipeline(const RasterizationPipelineInfo &info) : context(Application::instance->getContext()) {
        context.createRasterizationPipeline(handle, info);
    }

    ~RasterizationPipeline::RasterizationPipeline() {
        context.releaseRasterizationPipeline(handle);
    }
}
