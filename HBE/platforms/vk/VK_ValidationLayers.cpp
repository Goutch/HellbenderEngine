//
// Created by user on 2021-02-19.
//

#include <cstring>
#include "VK_ValidationLayers.h"
#include "vulkan/vulkan.h"
bool VK_ValidationLayers::checkValidationLayerSupport() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> availableLayers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, availableLayers.data());

    for (const char* layerName : validation_layers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

const std::vector<const char *>& VK_ValidationLayers::getLayersNames() {
    return validation_layers;
}

