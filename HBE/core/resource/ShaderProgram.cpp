
#include "ShaderProgram.h"
#include <graphics/gl/GL_ShaderProgram.h>

ShaderProgram *ShaderProgram::create() {
#if RENDERER == OPENGL_RENDERER
    return new GL_ShaderProgram();
#elif RENDERER == VULKAN_RENDERER

#endif
    return nullptr;
}