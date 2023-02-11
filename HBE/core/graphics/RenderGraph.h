#pragma once

#include "Core.h"
#include "Renderer.h"
#include "unordered_map"
namespace HBE {
	class GraphicPipeline;

	class GraphicPipelineInstance;

	class Mesh;

	class HB_API RenderGraph {
#define MAP(T1, T2) std::unordered_map<T1,T2>
		MAP(const GraphicPipeline*,
			MAP(GraphicPipelineInstance * ,
				MAP(const Mesh*, std::vector<DrawCmdInfo>))) render_cache;

		std::vector<DrawCmdInfo> ordered_render_cache;

		const size_t PUSH_CONSTANT_BLOCK_SIZE = 1024 * 16; //16kb
		size_t current_pc_block = 0;
		size_t current_pc_block_offset = 0;
		std::vector<char *> push_constant_blocks;
	public:
		RenderGraph();

		virtual ~RenderGraph();

		void draw(DrawCmdInfo draw_cmd_info);

		void clear();

		MAP(const GraphicPipeline*,
			MAP(GraphicPipelineInstance * ,
				MAP(const Mesh*, std::vector<DrawCmdInfo>))) &getRenderCache();

		std::vector<DrawCmdInfo> &getOrderedRenderCache();



	};
}

