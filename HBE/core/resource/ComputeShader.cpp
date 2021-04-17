#include "ComputeShader.h"
#include "platforms/gl/GL_ComputeShaderUtils.h"

namespace HBE {
    void HBE::ComputeShader::dispatch(int x, int y, int z) {

#if RENDERER == OPENGL_RENDERER
        GL_ComputeShaderUtils::dispatch(x, y, z);
#elif RENDERER == VULKAN_RENDERER
        Log::error("Colmpute shaders are not implemented in vulkan")
            return nullptr;
#endif
    }
}