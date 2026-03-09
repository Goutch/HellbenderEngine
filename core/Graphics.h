#pragma once
#ifdef VULKAN_GRAPHICS
#include "platforms/vk/VK_Context.h"
#define GRAPHIC_API_IMPL HBE::VK_Context
#else
#include "platforms/dummy/Dummy_Context.h"
#define GRAPHIC_API_IMPL HBE::Dummy_Context
#endif
#include "Context.h"
using Context = HBE::ContextBase<GRAPHIC_API_IMPL>;
