#pragma once

#include "core/resource/ComputeShader.h"
#include "GL_GraphicPipeline.h"

namespace HBE
{
    class GL_ComputeShaderUtils {
    public:
        static void dispatch(int x, int y, int z);
    };

}
