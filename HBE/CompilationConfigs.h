#pragma once
#define RESOURCE_PATH "../res/"
#define APPLICATION_NAME "PROTOTYPE"
#define WIDTH 900
#define HEIGHT 600
#define DEBUG_MODE true
enum {
    OPENGL_RENDERER=0,
    VULKAN_RENDERER=1,
};
#define RENDERER OPENGL_RENDERER