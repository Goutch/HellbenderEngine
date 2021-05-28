#include "vulkan/vulkan.h"
namespace HBE{
    class VK_Device;
    class VK_Semaphore {
        VkSemaphore handle;
        const VK_Device* device;
    public:
        VK_Semaphore(const VK_Device& device);
        ~VK_Semaphore();

        const VkSemaphore& getHandle();
    };
}
