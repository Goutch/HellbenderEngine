#include "RootAccelerationStructure.h"
#include "core/Application.h"

namespace HBE {

	RootAccelerationStructure::RootAccelerationStructure() : context(*Application::instance->getContext()) {

	}

	RootAccelerationStructure::~RootAccelerationStructure() {
		release();
	}

	void RootAccelerationStructure::alloc(const RootAccelerationStructureInfo &info) {
		context.createRootAccelerationStructure(handle, info);
	}

	void RootAccelerationStructure::release() {
		if (allocated()) {
			context.releaseRootAccelerationStructure(handle);
			handle = HBE_NULL_HANDLE;
		}
	}

	RootAccelerationStructureHandle RootAccelerationStructure::getHandle() {
		return handle;
	}

	RootAccelerationStructureHandle &RootAccelerationStructure::getHandleRef() {
		return handle;
	}

	bool RootAccelerationStructure::allocated() {
		return handle != HBE_NULL_HANDLE;
	}
}