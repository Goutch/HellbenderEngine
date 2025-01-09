//
// Created by User on 2/6/2023.
//

#include "RenderGraph.h"

#include <algorithm>

#include "core/resource/Mesh.h"
#include "core/resource/RasterizationPipelineInstance.h"
#include "core/resource/RaterizationPipeline.h"
#include "core/graphics/Renderer.h"
#include "core/utility/Profiler.h"

namespace HBE {
	RenderGraph::RenderGraph() {
		push_constant_blocks.emplace_back(static_cast<char *>(malloc(PUSH_CONSTANT_BLOCK_SIZE)));
	}

	RenderGraph::~RenderGraph() {
		for (int i = 0; i < push_constant_blocks.size(); ++i) {
			delete[] push_constant_blocks[i];
		}
	}

	void RenderGraph::add(DrawCmdInfo draw_cmd_info) {
		if (draw_cmd_info.mesh->getInstanceCount() == 0) return;
		//copy push constants data to cache
		for (int i = 0; i < draw_cmd_info.push_constants_count; ++i) {
			if (draw_cmd_info.push_constants[i].size + current_pc_block_offset > PUSH_CONSTANT_BLOCK_SIZE) {
				current_pc_block_offset = 0;
				current_pc_block++;
				if (current_pc_block >= push_constant_blocks.size()) {
					Log::debug("New Push Constant Block");
					push_constant_blocks.emplace_back(static_cast<char *>(malloc(PUSH_CONSTANT_BLOCK_SIZE)));
				}
			}
			void *cache_address = (void *) (&push_constant_blocks[current_pc_block][current_pc_block_offset]);
			memcpy(cache_address,
			       draw_cmd_info.push_constants[i].data,
			       draw_cmd_info.push_constants[i].size);
			current_pc_block_offset += draw_cmd_info.push_constants[i].size;
			draw_cmd_info.push_constants[i].data = cache_address;
		}

		//copy push constants to cache
		if ((sizeof(PushConstantInfo) * draw_cmd_info.push_constants_count) + current_pc_block_offset > PUSH_CONSTANT_BLOCK_SIZE) {
			current_pc_block_offset = 0;
			current_pc_block++;
			if (current_pc_block >= push_constant_blocks.size()) {
				push_constant_blocks.emplace_back(static_cast<char *>(malloc(PUSH_CONSTANT_BLOCK_SIZE)));
			}
		}
		void *cache_address = (void *) (&push_constant_blocks[current_pc_block][current_pc_block_offset]);
		memcpy(cache_address,
		       draw_cmd_info.push_constants,
		       sizeof(PushConstantInfo) * draw_cmd_info.push_constants_count);
		draw_cmd_info.push_constants = (PushConstantInfo *) cache_address;
		current_pc_block_offset += sizeof(PushConstantInfo) * draw_cmd_info.push_constants_count;
		if (draw_cmd_info.flags & DRAW_CMD_FLAG_ORDERED) {
			ordered_render_cache.push_back(draw_cmd_info);
		} else {
			render_cache.push_back(draw_cmd_info);
		}
	}

	void RenderGraph::clear() {
		current_pc_block_offset = 0;
		current_pc_block = 0;
		render_cache.clear();
		ordered_render_cache.clear();
	}

	const std::vector<DrawCmdInfo> &RenderGraph::getOrderedRenderCache() {
		/*HB_PROFILE_BEGIN("SortOrderedRenderCache");
		std::sort(ordered_render_cache.begin(), ordered_render_cache.end(), [](const DrawCmdInfo &cmd1, const DrawCmdInfo &cmd2) {
			return RenderGraph::compareDrawCmdOrdered(cmd1, cmd2);
		});
		HB_PROFILE_END("SortOrderedRenderCache");*/
		return ordered_render_cache;
	}

	const std::vector<DrawCmdInfo> &RenderGraph::getRenderCache() {
		HB_PROFILE_BEGIN("SortRenderCache");
		std::sort(render_cache.begin(), render_cache.end(), [](const DrawCmdInfo &cmd1, const DrawCmdInfo &cmd2) {
			return RenderGraph::compareDrawCmd(cmd1, cmd2);
		});
		HB_PROFILE_END("SortRenderCache");
		return render_cache;
	}

	bool RenderGraph::compareDrawCmdOrdered(const DrawCmdInfo &cmd1, const DrawCmdInfo &cmd2) {
		if (cmd1.layer == cmd2.layer) {
			return cmd1.order_in_layer < cmd2.order_in_layer;
		} else {
			return cmd1.layer < cmd2.layer;
		}
	}

	bool RenderGraph::compareDrawCmd(const DrawCmdInfo &cmd1, const DrawCmdInfo &cmd2) {
		const RaterizationPipeline *gp1 = cmd1.pipeline_instance->getGraphicPipeline();
		const RaterizationPipeline *gp2 = cmd2.pipeline_instance->getGraphicPipeline();
		if (gp1 != gp2) {
			return gp1 > gp2;
		} else {
			const RasterizationPipelineInstance *gpi1 = cmd1.pipeline_instance;
			const RasterizationPipelineInstance *gpi2 = cmd2.pipeline_instance;
			if (gpi1 != gpi2) {
				return gpi1 > gpi2;
			} else {
				const Mesh *m1 = cmd1.mesh;
				const Mesh *m2 = cmd2.mesh;
				return m1 > m2;
			}
		}
	}

}

