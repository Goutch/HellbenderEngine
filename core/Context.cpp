#include "Context.h"
#include "platforms/vk/VK_Context.h"

namespace HBE {
    void Context::init(const ContextInfo &info) {
        switch (info.api) {
            case GRAPHICS_API_VULKAN:
                context_data = new VK_Context(info);
                break;
            default: ;
        }
    }

    void Context::release() {
        switch (info.api) {
            case GRAPHICS_API_VULKAN:
                delete static_cast<VK_Context *>(context_data);
                break;
            default: ;
        }
    }
} // HBE
