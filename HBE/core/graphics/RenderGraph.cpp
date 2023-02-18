//
// Created by User on 2/6/2023.
//

#include "RenderGraph.h"
#include "core/resource/Mesh.h"
#include "core/resource/GraphicPipelineInstance.h"
#include "core/resource/GraphicPipeline.h"
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

    void RenderGraph::draw(DrawCmdInfo draw_cmd_info) {
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
            auto pipeline_it = render_cache.find(draw_cmd_info.pipeline_instance->getGraphicPipeline());
            if (pipeline_it == render_cache.end())
                pipeline_it = render_cache.emplace(draw_cmd_info.pipeline_instance->getGraphicPipeline(),
                                                   MAP(GraphicPipelineInstance*, MAP(const Mesh*, std::vector<DrawCmdInfo>))()).first;
            auto material_it = pipeline_it->second.find(draw_cmd_info.pipeline_instance);
            if (material_it == pipeline_it->second.end())
                material_it = pipeline_it->second.emplace(draw_cmd_info.pipeline_instance, MAP(const Mesh*, std::vector<DrawCmdInfo>)()).first;
            auto mesh_it = material_it->second.find(draw_cmd_info.mesh);
            if (mesh_it == material_it->second.end())
                mesh_it = material_it->second.emplace(draw_cmd_info.mesh, std::vector<DrawCmdInfo>()).first;

            mesh_it->second.emplace_back(draw_cmd_info);
        }
    }

    void RenderGraph::clear() {
        current_pc_block_offset = 0;
        current_pc_block = 0;
        render_cache.clear();
        ordered_render_cache.clear();
    }

    const std::vector<DrawCmdInfo> &RenderGraph::getOrderedRenderCache() {
        return ordered_render_cache;
    }

    const MAP(const GraphicPipeline*,
              MAP(GraphicPipelineInstance * ,
                  MAP(const Mesh*, std::vector<DrawCmdInfo>))) &RenderGraph::getRenderCache() {
        return render_cache;
    }


}

