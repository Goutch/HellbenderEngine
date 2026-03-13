#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "glm/glm.hpp"
#include <string>
#include "core/interface/MeshInterface.h"
#include "core/interface/RasterizationPipelineInterface.h"
#include "core/interface/RasterizationTargetInterface.h"
#include "core/interface/PipelineInstanceInterface.h"
#include "core/interface/FenceInterface.h"
namespace HBE {
    class RenderGraph;

    typedef uint32_t DRAW_CMD_FLAGS;

    enum DRAW_CMD_FLAG {
        DRAW_CMD_FLAG_NONE = 0,
        DRAW_CMD_FLAG_ORDERED = 1 << 0,
    };

    typedef uint32_t RASTERIZE_CMD_FLAGS;

    enum RASTERIZE_CMD_FLAG {
        RASTERIZE_CMD_FLAG_NONE = 0,
        RASTERIZE_CMD_FLAG_INVERSE_Y = 1 << 0,
    };

    typedef uint32_t TRACE_RAYS_CMD_FLAGS;

    enum TRACE_RAYS_CMD_FLAG {
        TRACE_RAYS_CMD_FLAG_NONE = 0,
    };

    typedef uint32_t PRESENT_CMD_FLAGS;

    enum PRESENT_CMD_FLAG {
        PRESENT_CMD_FLAG_NONE = 0,
    };

    struct PushConstantInfo {
        std::string name;
        //array of 128bytes max
        const void *data;
        uint32_t size;
    };

    struct DrawCmdInfo {
        MeshHandle mesh = HBE_NULL_HANDLE;
        RasterizationPipelineHandle rasterization_pipeline_handle = HBE_NULL_HANDLE;
        PipelineInstanceHandle pipeline_instance_handle = HBE_NULL_HANDLE;
        uint32_t layer = 0;
        uint32_t order_in_layer = 0; //ignored if ordered flag is not set
        uint32_t push_constants_count = 0;
        PushConstantInfo *push_constants = nullptr;
        DRAW_CMD_FLAGS flags = DRAW_CMD_FLAG_NONE;
    };

    struct RasterizeGraphCmdInfo {
        RasterizationTargetHandle rasterization_target_handle;
        RenderGraph *render_graph;
        uint32_t layer_mask;
        mat4 view;
        mat4 projection;
        RASTERIZE_CMD_FLAGS flags;
    };

    struct TraceRaysCmdInfo {
        PipelineInstanceHandle pipeline_instance;
        vec2i resolution;
        TRACE_RAYS_CMD_FLAGS flags;
    };

    struct PresentCmdInfo {
        ImageHandle *images;
        uint32_t image_count;
        PRESENT_CMD_FLAGS flags;
    };

    struct ComputeDispatchCmdInfo {
        PipelineInstanceHandle pipeline_instance;
        uint32_t size_x = 1;
        uint32_t size_y = 1;
        uint32_t size_z = 1;
        //used only for async compute right now
        FenceHandle fence;
    };

	struct RendererResources{
		RasterizationTargetHandle main_render_target = HBE_NULL_HANDLE;
		RasterizationTargetHandle ui_render_target = HBE_NULL_HANDLE;
		PipelineInstanceHandle screen_pipeline_instance = HBE_NULL_HANDLE;
	    RasterizationPipelineHandle screen_pipeline = HBE_NULL_HANDLE;
	};
}
