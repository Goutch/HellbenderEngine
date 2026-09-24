#pragma once
namespace HBE {
    enum GRAPHICS_API
    {
        GRAPHICS_API_NONE,
        GRAPHICS_API_VULKAN,
    };

    typedef uint32_t VULKAN_REQUIRED_EXTENSION_FLAGS;

    enum VULKAN_REQUIRED_EXTENSIONS_FLAG
    {
        VULKAN_REQUIRED_EXTENSION_NONE = 0,
        VULKAN_REQUIRED_EXTENSION_RTX = 1,
        VULKAN_REQUIRED_EXTENSION_DESCRIPTOR_INDEXING = 2,
    };

    enum VULKAN_VERSION
    {
        VULKAN_VERSION_1_0 = 0,
        VULKAN_VERSION_1_1 = 1,
        VULKAN_VERSION_1_2 = 2,
        VULKAN_VERSION_1_3 = 3,
    };

    struct ContextInfo
    {
        GRAPHICS_API api = GRAPHICS_API_VULKAN;
        VULKAN_REQUIRED_EXTENSION_FLAGS required_extension_flags = VULKAN_REQUIRED_EXTENSION_NONE;
        VULKAN_VERSION vulkan_version = VULKAN_VERSION_1_0;
    };

}