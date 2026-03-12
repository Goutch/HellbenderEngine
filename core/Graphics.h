#pragma once
#include "Context.h"
#ifdef VULKAN_GRAPHICS
    #include "platforms/vk/VK_Context.h"
    #define GRAPHICS_API_IMPLEMENTATION HBE::VK_Context
#else
    #include "platforms/dummy/Dummy_Context.h"
    #define GRAPHICS_API_IMPLEMENTATION HBE::Dummy_Context
#endif

namespace HBE {
    typedef ContextBase<GRAPHICS_API_IMPLEMENTATION> Context;
}
