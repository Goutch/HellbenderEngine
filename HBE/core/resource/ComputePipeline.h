#pragma once
#include "Core.h"
#include "Resource.h"

namespace HBE {
    class Shader;

    class ComputeBuffer;

    struct ComputePipelineInfo{
    	Shader* shader;
    };
    class HB_API ComputePipeline : public Resource {
        virtual void setShader(const Shader *shader) = 0;

        virtual void setComputeBuffer(ComputeBuffer* buffer)=0;

        virtual void dispatch(int x, int y, int z) = 0;
    };
}
