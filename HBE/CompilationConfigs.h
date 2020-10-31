#pragma once
#define RESOURCE_PATH "../dependencies/HellbenderEngine/res/"
#define APPLICATION_NAME "PROTOTYPE"
#define WIDTH 640
#define HEIGHT 640
#define DEBUG_MODE true
enum {
    OPENGL_RENDERER=0,
    VULKAN_RENDERER=1,
};
#define RENDERER OPENGL_RENDERER