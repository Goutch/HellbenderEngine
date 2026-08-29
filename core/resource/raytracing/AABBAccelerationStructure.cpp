
#include "AABBAccelerationStructure.h"
#include "core/Application.h"

namespace HBE {
	AABBAccelerationStructure::AABBAccelerationStructure() : context(*Application::instance->getContext()) {

	}

	AABBAccelerationStructure::~AABBAccelerationStructure() {
		release();
	}

	void AABBAccelerationStructure::alloc(const AABBAccelerationStructureInfo &info) {
		context.createAABBAccelerationStructure(handle,info);
	}

	void AABBAccelerationStructure::release() {
		if (allocated()) {
			context.releaseAABBAccelerationStructure(handle);
			handle = HBE_NULL_HANDLE;
		}
	}

	AABBAccelerationStructureHandle AABBAccelerationStructure::getHandle() {
		return handle;
	}

	AABBAccelerationStructureHandle &AABBAccelerationStructure::getHandleRef() {
		return handle;
	}

	bool AABBAccelerationStructure::allocated() {
		return handle != HBE_NULL_HANDLE;
	}
}