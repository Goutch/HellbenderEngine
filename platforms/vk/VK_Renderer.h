#pragma once
#include "core/graphics/Renderer.h"
#include "vulkan/vulkan.h"
#include "VK_Semaphore.h"
#include "array"
#include "VK_CommandPool.h"
#include "dependencies/utils-collection/Event.h"

namespace HBE
{
    class VK_Window;

    class VK_Instance;

    class VK_PhysicalDevice;

    class VK_Device;

    class VK_Surface;

    class VK_Swapchain;

    class VK_RenderPass;

    class VK_CommandPool;

    class VK_Fence;

    class VK_Renderer : public Renderer
    {
        struct FrameState
        {
            VK_Semaphore* finished_semaphore;
            VK_Semaphore* image_available_semaphore;
        };

        uint32_t current_frame = 0;
        uint32_t current_image = 0;
        std::array<FrameState, MAX_FRAMES_IN_FLIGHT> frames;
        std::vector<VK_Fence*> images_in_flight_fences;

        VkSampler default_sampler;
        RasterizationTarget* main_render_target = nullptr;
        RasterizationTarget* ui_render_target = nullptr;
        RasterizationPipeline* screen_pipeline = nullptr;
        RasterizationPipelineInstance* screen_pipeline_instance = nullptr;
        bool windowResized = false;
        bool frame_presented = false;

        event_subscription_id vertical_sync_changed_subscription_id;
        event_subscription_id window_closed_subscription_id;
        event_subscription_id window_size_changed_subscription_id;

        VK_CommandPool command_pool;
        VK_Device* device = nullptr;
        VK_Swapchain* swapchain = nullptr;

    public:
        void init(VK_Device& device, VK_Swapchain& swapchain);
        void release();

        VK_Renderer() = default;
        ~VK_Renderer() = default;
        VK_Renderer(const VK_Renderer&) = delete;
        VK_Renderer& operator=(const VK_Renderer&) = delete;


        void rasterize(RasterizeCmdInfo& render_cmd_info) override;

        void traceRays(TraceRaysCmdInfo& trace_rays_cmd_info) override;

        void present(PresentCmdInfo& present_cmd_info) override;

        void waitCurrentFrame() override;
        void waitLastFrame() override;

        RasterizationTarget* getDefaultRenderTarget() override;

        RasterizationTarget* getUIRenderTarget() override;

        void beginFrame() override;

        void endFrame() override;

        VK_CommandPool* getCommandPool();

        VK_Device* getDevice();

        uint32_t getFrameCount() const override;

        void onWindowClosed();

        void onWindowSizeChange(Window* window);

        void reCreateSwapChain();

        const VK_Swapchain& getSwapChain() const;

        uint32_t getCurrentFrameIndex() const override;

        void waitAll();

        VkSampler getDefaultSampler();

        GraphicLimits getLimits() override;

        void computeDispatch(ComputeDispatchCmdInfo& compute_dispatch_cmd_info) override;

        Fence* getLastFrameFence() override;

        Fence* getCurrentFrameFence() override;

    private:
        void createDefaultResources() override;
    };
}
