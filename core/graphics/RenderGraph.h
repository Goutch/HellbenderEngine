#pragma once

#include "Core.h"
#include "Renderer.h"
#include "unordered_map"
#include "vector"

namespace HBE {
	class GraphicPipeline;

	class GraphicPipelineInstance;

	class Mesh;

	class HB_API RenderGraph {
#define MAP(T1, T2) std::unordered_map<T1,T2>
		std::vector<DrawCmdInfo> render_cache;
		std::vector<DrawCmdInfo> ordered_render_cache;

		const size_t PUSH_CONSTANT_BLOCK_SIZE = 1024 * 64;
		size_t current_pc_block = 0;
		size_t current_pc_block_offset = 0;
		std::vector<char *> push_constant_blocks;

		static bool compareDrawCmd(const DrawCmdInfo &cmd1, const DrawCmdInfo &cmd2);

		static bool compareDrawCmdOrdered(const DrawCmdInfo &cmd1, const DrawCmdInfo &cmd2);

	public:
		RenderGraph();

		~RenderGraph();

		void draw(DrawCmdInfo draw_cmd_info);

		void clear();

		const std::vector<DrawCmdInfo> &getRenderCache();

		const std::vector<DrawCmdInfo> &getOrderedRenderCache();


	};
}

