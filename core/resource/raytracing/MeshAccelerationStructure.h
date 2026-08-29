
#pragma once
#include "Core.h"
#include "core/Graphics.h"
#include "core/interface/MeshAccelerationStructureInterface.h"
namespace HBE{
	class HB_API MeshAccelerationStructure {
		MeshAccelerationStructureHandle handle;
		Context& context;
	public:
		MeshAccelerationStructure();
		~MeshAccelerationStructure();
		void alloc(const MeshAccelerationStructureInfo& info);
		void release();
		MeshAccelerationStructureHandle getHandle();
		MeshAccelerationStructureHandle &getHandleRef();
		bool allocated();
	};
}

