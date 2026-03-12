#include "TextSystem.h"

#include "core/resource/Font.h"
#include "core/scene/systems/MeshRendererSystem.h"
#include "core/resource/Shader.h"
#include "core/resource/RasterizationPipeline.h"
#include "core/interface/PipelineInstanceInterface.h"
#include "core/scene/components/Transform.h"
#include "core/scene/Scene.h"
#include "core/scene/components/Node.h"
#include "core/utility/Geometry.h"
#include "core/Context.h"

namespace HBE
{
    void TextSystem::onAttachLabel(Entity text_entity)
    {
        TextRenderer* text_component = text_entity.get<TextRenderer>();

        text_component->pipeline_instance = default_text_pipeline_instance.getHandle();
        text_component->font = &default_font;
        text_component->height = 40;
    }

    TextSystem::~TextSystem()
    {
        scene->onAttach<TextRenderer>().unsubscribe(on_attach_subscription_id);
        scene->onDetach<TextRenderer>().unsubscribe(on_detach_subscription_id);
        scene->onDraw.unsubscribe(on_draw_subscription_id);
    }

    TextSystem::TextSystem(Scene* scene, RasterizationTarget* render_target) : System(scene)
    {
        scene->onAttach<TextRenderer>().subscribe(on_attach_subscription_id, this, &TextSystem::onAttachLabel);
        scene->onDetach<TextRenderer>().subscribe(on_detach_subscription_id, this, &TextSystem::onDetachLabel);
        scene->onDraw.subscribe(on_draw_subscription_id, this, &TextSystem::onPrepareRenderGraph);

        std::string characters =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
        FontInfo font_info{};
        font_info.path = "fonts/Roboto-Regular.ttf";
        font_info.characters = characters.data();
        font_info.characters_count = characters.size();
        font_info.glyph_resolution = 64;

        default_font.load(font_info);

        default_text_vert_shader.loadGLSL("shaders/defaults/ui/Label.vert", SHADER_STAGE_VERTEX);
        default_text_frag_shader.loadGLSL("shaders/defaults/ui/Label.frag", SHADER_STAGE_FRAGMENT);


        RasterizationPipelineInfo text_pipeline_info{};
        text_pipeline_info.vertex_shader = default_text_vert_shader.getHandle();
        text_pipeline_info.fragment_shader = default_text_frag_shader.getHandle();
        text_pipeline_info.rasterization_target = render_target->getHandle();
        text_pipeline_info.attribute_info_count = 1;
        text_pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
        text_pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NO_DEPTH_TEST;
        default_text_pipeline.alloc(text_pipeline_info);
        default_text_pipeline.allocInstance(default_text_pipeline_instance);

        default_text_pipeline_instance.setUniform("material", &DEFAULT_TEXT_COLOR);
        default_text_pipeline_instance.setImage("mtsdf", default_font.getTextureAtlas()->getHandle());
    }

    void TextSystem::onDetachLabel(Entity text_entity)
    {
        TextRenderer* text_component = text_entity.get<TextRenderer>();
        if (text_component->mesh!=HBE_NULL_HANDLE)
        {
            context.releaseMesh(text_component->mesh);
        }
    }

    void TextSystem::onPrepareRenderGraph(RenderGraph* render_graph)
    {
        HB_PROFILE_BEGIN("TextComponentDraw");
        auto group = scene->group<Node, Transform, TextRenderer>();

        for (auto [handle, node, transform, text_component] : group)
        {
            if (!text_component.active ||
                text_component.pipeline_instance == HBE_NULL_HANDLE ||
                text_component.font == nullptr ||
                !node.isActiveInHierarchy())
            {
                continue;
            }

            if (text_component.dirty)
            {
                if (text_component.getText().length() == 0 && text_component.mesh != HBE_NULL_HANDLE)
                {
                    context.releaseMesh(text_component.mesh);
                    continue;
                }
                Geometry::updateText(context,
                                    text_component.mesh,
                                     text_component.getText(),
                                     *text_component.font,
                                     1.0f,
                                     0.5f,
                                     text_component.alignment,
                                     text_component.anchor,
                                     text_component.size);
            }
            HB_ASSERT(text_component.pipeline_instance != HBE_NULL_HANDLE, "graphic pipeline instance is not set");

            DrawCmdInfo cmd{};
            cmd.mesh = text_component.mesh;
            cmd.pipeline_instance_handle = text_component.pipeline_instance;
            cmd.order_in_layer = node.getGlobalIndex();
            LabelPushConstant push_constant{};
            push_constant.text_height = text_component.height;
            push_constant.world_matrix = transform.world();

            PushConstantInfo push_constant_info{};
            push_constant_info.name = "constants";
            push_constant_info.size = sizeof(mat4) + sizeof(float);
            push_constant_info.data = &push_constant;
            cmd.push_constants = &push_constant_info;
            cmd.push_constants_count = 1;
            cmd.flags = DRAW_CMD_FLAG_ORDERED;
            cmd.layer = text_component.layer;
            cmd.order_in_layer = node.getGlobalIndex();

            render_graph->add(cmd);
        }
        HB_PROFILE_END("TextComponentDraw");
    }

    void TextRenderer::setText(const std::string& text)
    {
        this->text = text;
        dirty = true;
    }

    const std::string& TextRenderer::getText() const
    {
        return text;
    }

    vec2 TextRenderer::getSize() const
    {
        return size;
    }
}
