#pragma once
#include "../../core/interface/RendererInterface.h"
#include "vulkan/vulkan.h"
#include "VK_Semaphore.h"
#include "array"
#include "VK_CommandPool.h"
#include "core/graphics/GraphicLimits.h"
#include "dependencies/utils-collection/Event.h"

namespace HBE {
    class Window;
    class VK_Window;

    class VK_Instance;

    class VK_PhysicalDevice;

    class VK_Device;

    class VK_Surface;

    class VK_Swapchain;

    class VK_RenderPass;

    class VK_CommandPool;

    class VK_Fence;

    class VK_Renderer {
        struct FrameState {
            VK_Semaphore *finished_semaphore;
            VK_Semaphore *image_available_semaphore;
        };

        uint32_t current_frame_index = 0;
        uint32_t current_image = 0;
        std::array<FrameState, MAX_FRAMES_IN_FLIGHT> frames;
        std::vector<VK_Fence *> images_in_flight_fences;

        VkSampler default_sampler;
        RasterizationTargetHandle main_render_target = HBE_NULL_HANDLE;
        RasterizationTargetHandle ui_render_target = HBE_NULL_HANDLE;
        RasterizationPipelineHandle screen_pipeline = HBE_NULL_HANDLE;
        PipelineInstanceHandle screen_pipeline_instance;
        bool windowResized = false;
        bool frame_presented = false;

        event_subscription_id vertical_sync_changed_subscription_id;
        event_subscription_id window_closed_subscription_id;
        event_subscription_id window_size_changed_subscription_id;

        VK_CommandPool command_pool;
        VK_Context *context = nullptr;

    public:
        Event<uint32_t> onFrameEnd;

        void init(VK_Context *context);

        void release();


        VK_Renderer() = default;

        ~VK_Renderer() = default;

        VK_Renderer(const VK_Renderer &) = delete;

        VK_Renderer &operator=(const VK_Renderer &) = delete;

        void cmdRasterizeGraph(const RasterizeGraphCmdInfo& info);

        void cmdTraceRays(const TraceRaysCmdInfo& trace_rays_cmd_info);

        void cmdPresent(const PresentCmdInfo &present_cmd_info);

        void waitCurrentFrame();

        void waitLastFrame();

        RasterizationTarget *getDefaultRenderTarget();

        RasterizationTarget *getUIRenderTarget();

        void beginFrame();

        void endFrame();

        VK_CommandPool *getCommandPool();

        uint32_t getFrameCount() const;

        void onWindowClosed();

        void onWindowSizeChange(Window *window);

        void reCreateSwapChain();

        uint32_t getCurrentFrameIndex() const;

        void waitAll();

        VkSampler getDefaultSampler();

        GraphicLimits getLimits();

        void cmdDispatch(const ComputeDispatchCmdInfo &compute_dispatch_cmd_info);

        Fence *getLastFrameFence();

        Fence *getCurrentFrameFence();

    private:
        void createDefaultResources();
    };
}
