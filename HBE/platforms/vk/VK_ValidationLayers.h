#pragma once
#include "vector"
#include "Core.h"
#include "vulkan/vulkan.h"
class VK_ValidationLayers {
    const std::vector<const char*> validation_layers = {
            "VK_LAYER_KHRONOS_validation"
    };

    static bool checkValidationLayerSupport();

public:
    const std::vector<const char*> getLayersNames();
};
