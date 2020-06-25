#pragma once
#define RESOURCE_PATH "../dependencies/InsertName_Framework/res/"
#define APPLICATION_NAME "PROTOTYPE"
#define WIDTH 800
#define HEIGHT 600
#define DEBUG_MODE true
enum {
    OPENGL_RENDERER=0,
    VULKAN_RENDERER,
};
#define RENDERER OPENGL_RENDERER