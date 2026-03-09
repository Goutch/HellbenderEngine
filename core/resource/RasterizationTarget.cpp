#pragma once
#include "RasterizationTarget.h"

#include "core/Application.h"

namespace HBE
{
    RasterizationTarget::RasterizationTarget() : context(*Application::instance->getContext())
    {
    }

    RasterizationTarget::RasterizationTarget(const RasterizationTargetInfo& info): context(*Application::instance->getContext())
    {
        context.createRasterizationTarget(handle, info);
    }

    void RasterizationTarget::setResolution(vec2u resolution)
    {
        context.setRasterizationTargetResolution(handle, resolution);
    }

    vec2u RasterizationTarget::getResolution()
    {
        vec2u resolution;
        context.getRasterizationTargetResolution(handle, resolution);
        return resolution;
    }
}
