#pragma once

#include "vulkan/vulkan.h"
#include "optional"
#include "unordered_map"
#include "set"
#include "map"

namespace HBE
{
    typedef uint32_t EXTENSION_FLAGS;

    enum EXTENSION_FLAG
    {
        EXTENSION_FLAG_NONE = 0,
        EXTENSION_FLAG_RAY_TRACING_PIPELINE = 1 << 0,
        EXTENSION_FLAG_ACCELERATION_STRUCTURE = 1 << 1,
        EXTENSION_FLAG_BUFFER_DEVICE_ADDRESS = 1 << 2,
        EXTENSION_FLAG_DESCRIPTOR_INDEXING = 1 << 3,
        EXTENSION_FLAG_DYNAMIC_RENDERING = 1 << 4,
        EXTENSION_FLAG_64BIT_INTEGER_GLSL_TYPE = 1 << 5,
        EXTENSION_FLAG_NULL_DESCRIPTORS = 1 << 6,
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;
        std::optional<uint32_t> compute_family;
        std::optional<uint32_t> transfer_family;

        bool hasMinimumRequirements()
        {
            return graphics_family.has_value() &&
                present_family.has_value();
        }

        bool isComplete()
        {
            return graphics_family.has_value() &&
                present_family.has_value() &&
                compute_family.has_value() &&
                transfer_family.has_value();
        }
    };

    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    class VK_PhysicalDevice
    {
        const std::vector<const char*> REQUIRED_EXTENSIONS = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        const std::vector<const char*> RAYTRACING_EXTENSIONS_1_1 = {
            VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
            VK_KHR_SPIRV_1_4_EXTENSION_NAME,
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
            VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        };
        const std::vector<const char*> RAYTRACING_EXTENSIONS_1_2 = {
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
            VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
            //VK_KHR_RAY_QUERY_EXTENSION_NAME
        };

        const std::vector<const char*> DESCRIPTOR_INDEXING_EXTENSIONS_1_1 = {
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
        };

        const std::vector<const char*> DEBUG_EXTENSIONS = {
            VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
        };

        VkInstance vk_instance_handle;
        VkSurfaceKHR surface_handle;
        VkPhysicalDevice handle = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        QueueFamilyIndices queue_family_indices{};
        SwapchainSupportDetails support_details{};
        VkPhysicalDeviceMemoryProperties memory_properties{};
        VkFormatProperties format_properties{};

        VkPhysicalDeviceAccelerationStructurePropertiesKHR acceleration_structure_properties{};
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_pipeline_properties{};
        VkPhysicalDeviceDescriptorIndexingProperties descriptor_indexing_properties{};


        VkPhysicalDeviceRobustness2FeaturesEXT robustness2_features{};
        VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features{};
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_pipeline_features{};
        VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing_features{};
        VkPhysicalDeviceBufferDeviceAddressFeatures buffer_device_address_features{};
        VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features{};

        std::vector<const char*> enabled_extensions;
        std::multimap<uint32_t, VkPhysicalDevice> device_score;
        std::unordered_map<VkPhysicalDevice, std::vector<const char*>> device_enabled_extensions;

        EXTENSION_FLAGS enabled_extension_flags = EXTENSION_FLAG_NONE;

    public:
        VK_PhysicalDevice(VkInstance vk_instance_handle, VkSurfaceKHR surface_handle);

        const QueueFamilyIndices& getQueueFamilyIndices() const;

        const VkPhysicalDevice& getHandle() const;


        const std::vector<const char*>& getExtensions() const;

        const SwapchainSupportDetails& getSwapchainSupportDetails() const;

        SwapchainSupportDetails querySwapchainSupportDetails(const VkPhysicalDevice& physical_device) const;

        const VkPhysicalDeviceFeatures& getFeatures() const;

        const VkPhysicalDeviceAccelerationStructureFeaturesKHR& getAccelerationStructureFeatures() const;

        const VkPhysicalDeviceRayTracingPipelineFeaturesKHR& getRayTracingPipelineFeatures() const;

        const VkPhysicalDeviceBufferDeviceAddressFeatures& getBufferDeviceAddressFeatures() const;

        const VkPhysicalDeviceDescriptorIndexingFeatures& getDescriptorIndexingFeatures() const;

        const VkPhysicalDeviceProperties& getProperties() const;

        const VkFormatProperties& getFormatProperties() const;

        const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const;

        const VkPhysicalDeviceRayTracingPipelinePropertiesKHR& getRayTracingPipelineProperties() const;

        const VkPhysicalDeviceRobustness2FeaturesEXT getRobustnessFeatures();

        const VkPhysicalDeviceAccelerationStructurePropertiesKHR& getAccelerationStructureProperties() const;

        const EXTENSION_FLAGS& getEnabledExtensionFlags() const;

    private:
        void pickBestPhysicalDevice();

        bool checkExtensionsSupport(const VkPhysicalDevice& physical_device,
                                    const std::vector<const char*>& extensions);

        QueueFamilyIndices getSupportedQueueFamilies(const VkPhysicalDevice& physical_device);

        bool isDeviceSuitable(const VkPhysicalDevice& physical_device);

        void calculateScore(const VkPhysicalDevice& physical_device);
    };
}
