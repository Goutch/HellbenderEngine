
#include "MeshAccelerationStructure.h"
#include "core/Application.h"

namespace HBE {

	MeshAccelerationStructure::MeshAccelerationStructure() : context(*Application::instance->getContext()) {

	}

	MeshAccelerationStructure::~MeshAccelerationStructure() {
		release();
	}

	void MeshAccelerationStructure::alloc(const MeshAccelerationStructureInfo &info) {
		context.createMeshAccelerationStructure(handle, info);
	}

	void MeshAccelerationStructure::release() {
		context.releaseMeshAccelerationStructure(handle);
		handle = HBE_NULL_HANDLE;
	}

	MeshAccelerationStructureHandle MeshAccelerationStructure::getHandle() {
		return handle;
	}

	MeshAccelerationStructureHandle &MeshAccelerationStructure::getHandleRef() {
		return handle;
	}

	bool MeshAccelerationStructure::allocated() {
		return handle != HBE_NULL_HANDLE;
	}
}