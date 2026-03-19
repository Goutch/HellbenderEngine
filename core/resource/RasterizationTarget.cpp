#pragma once

#include "RasterizationTarget.h"

#include "core/Application.h"

namespace HBE {
	RasterizationTarget::RasterizationTarget() : context(*Application::instance->getContext()) {
	}

	RasterizationTarget::RasterizationTarget(const RasterizationTargetInfo &info) : context(*Application::instance->getContext()) {
		alloc(info);
	}

	RasterizationTargetHandle RasterizationTarget::getHandle() {
		return handle;
	}

	void RasterizationTarget::setResolution(vec2u resolution) {
		context.setRasterizationTargetResolution(handle, resolution);
	}

	vec2u RasterizationTarget::getResolution() {
		vec2u resolution;
		context.getRasterizationTargetResolution(handle, resolution);
		return resolution;
	}

	ImageHandle RasterizationTarget::getFramebufferTexture() {
		return context.getRasterizationTargetFrameBuffer(handle);
	}

	RasterizationTarget::~RasterizationTarget() {
		release();
	}

	void RasterizationTarget::release() {
		context.releaseRasterizationTarget(handle);
		handle = HBE_NULL_HANDLE;
	}

	bool RasterizationTarget::allocated() {
		return handle != HBE_NULL_HANDLE;
	}

	void RasterizationTarget::alloc(const RasterizationTargetInfo &info) {
		context.createRasterizationTarget(handle, info);
	}
}
