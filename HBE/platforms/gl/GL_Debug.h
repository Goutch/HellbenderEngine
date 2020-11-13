#pragma once
#include "glad/glad.h"
#include "iostream"
class GL_Debug {

public:
    static GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)
    static void APIENTRY glDebugOutput(GLenum source,
                                GLenum type,
                                unsigned int id,
                                GLenum severity,
                                GLsizei length,
                                const char *message,
                                       const void *userParam);
    static void init();
};


