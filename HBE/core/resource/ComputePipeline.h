#pragma once

#include "Resource.h"

namespace HBE {
    class Shader;

    class ComputeBuffer;

    class ComputePipeline : public Resource {
        virtual void setShader(const Shader *shader) = 0;

        virtual void setComputeBuffer(ComputeBuffer* buffer)=0;

        virtual void dispatch(int x, int y, int z) = 0;
    };
}