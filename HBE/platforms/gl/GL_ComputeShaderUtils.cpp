//
// Created by user on 4/17/2021.
//

#include "GL_ComputeShaderUtils.h"
#include "glad/glad.h"
void HBE::GL_ComputeShaderUtils::dispatch(int x, int y, int z) {
    glDispatchCompute((GLuint)x, (GLuint)y,(GLuint)z);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}


