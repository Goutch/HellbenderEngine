
#pragma once
#include "Core.h"
#include "core/Graphics.h"
#include "core/interface/AABBAccelerationStructureInterface.h"
namespace HBE {
	class HB_API AABBAccelerationStructure {
		AABBAccelerationStructureHandle handle;
		Context &context;
	public:
		AABBAccelerationStructure();
		~AABBAccelerationStructure();
		void alloc(const AABBAccelerationStructureInfo &info);
		void release();
		AABBAccelerationStructureHandle getHandle();
		AABBAccelerationStructureHandle &getHandleRef();
		bool allocated();
	};
}