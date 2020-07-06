#pragma once
#define RESOURCE_PATH "../dependencies/HellbenderEngine/res/"
#define APPLICATION_NAME "PROTOTYPE"
#define WIDTH 800
#define HEIGHT 800
#define DEBUG_MODE true
enum {
    OPENGL_RENDERER=0,
    VULKAN_RENDERER,
};
#define RENDERER OPENGL_RENDERER