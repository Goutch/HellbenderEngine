#pragma once
#include "core/Graphics.h"
#include "Core.h"
#include "core/interface/MeshAccelerationStructureInterface.h"

namespace HBE
{
    class HB_API MeshAccelerationStructure
    {
        MeshAccelerationStructureHandle handle;
        Context& context;
        public:
        MeshAccelerationStructure();
        ~MeshAccelerationStructure();
        void alloc();
        void release();
    };


    class HB_API AABBAccelerationStructure
    {
        AABBAccelerationStructureHandle handle;
        Context& context;
    public:
        AABBAccelerationStructure();
        ~AABBAccelerationStructure();
        void alloc();
        void release();
    };


    class HB_API RootAccelerationStructure
    {
        RootAccelerationStructureHandle handle;
        Context& context;
    public:
        RootAccelerationStructure();
        ~RootAccelerationStructure();
        void alloc();
        void release();
    };
}
