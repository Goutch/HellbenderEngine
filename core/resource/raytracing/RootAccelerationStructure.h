#pragma once
#include "core/Graphics.h"
#include "Core.h"
#include "core/interface/RootAccelerationStructureInterface.h"

namespace HBE
{
    class HB_API RootAccelerationStructure
    {
        RootAccelerationStructureHandle handle;
        Context& context;
    public:
        RootAccelerationStructure();
	    ~RootAccelerationStructure();
	    void alloc(const RootAccelerationStructureInfo &info);
	    void release();
	    RootAccelerationStructureHandle getHandle();
	    RootAccelerationStructureHandle &getHandleRef();
		bool allocated();
    };

}
