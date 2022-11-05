#pragma once

#include "HBE.h"

using namespace HBE;

class TextRenderingScene {
    std::string TEXT = "Hello world!\nThis is a test of the text rendering system";
public:
    TextRenderingScene() {
        Shader *text_vertex_shader;
        Shader *text_fragment_shader;
        Shader *background_vertex_shader;
        Shader *background_fragment_shader;
        ShaderInfo vertex_shader_info{};
        ShaderInfo fragment_shader_info{};
        vertex_shader_info.stage = SHADER_STAGE_VERTEX;
        fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;

        vertex_shader_info.path = "shaders/defaults/TextMSDF.vert";
        fragment_shader_info.path = "shaders/defaults/TextMSDF.frag";
        text_vertex_shader = Resources::createShader(vertex_shader_info, "text_vertex");
        text_fragment_shader = Resources::createShader(fragment_shader_info, "text_fragment");

        vertex_shader_info.path = "shaders/defaults/Position.vert";
        fragment_shader_info.path = "shaders/defaults/Position.frag";
        background_fragment_shader = Resources::createShader(fragment_shader_info, "background_vertex");
        background_vertex_shader = Resources::createShader(vertex_shader_info, "background_fragment");

        GraphicPipeline *text_pipeline;
        GraphicPipeline *background_pipeline;
        GraphicPipelineInfo pipeline_info{};
        pipeline_info.attribute_info_count = 1;

        pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
        pipeline_info.vertex_shader = text_vertex_shader;
        pipeline_info.fragement_shader = text_fragment_shader;
        pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_CULL_BACK;
        text_pipeline = Resources::createGraphicPipeline(pipeline_info, "text_pipeline");

        pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
        pipeline_info.vertex_shader = background_vertex_shader;
        pipeline_info.fragement_shader = background_fragment_shader;
        background_pipeline = Resources::createGraphicPipeline(pipeline_info, "background_pipeline");

        GraphicPipelineInstance *text_pipeline_instance;
        GraphicPipelineInstance *background_pipeline_instance;
        GraphicPipelineInstanceInfo pipeline_instance_info{};
        pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

        pipeline_instance_info.graphic_pipeline = text_pipeline;
        text_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info, "text_pipeline_instance");

        pipeline_instance_info.graphic_pipeline = background_pipeline;
        background_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info, "background_pipeline_instance");

        Scene *scene = Application::getScene();

        Entity camera_entity = scene->createEntity3D();

        Camera2D camera2D = camera_entity.attach<Camera2D>();

        scene->setCameraEntity(camera_entity);

        std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
        FontInfo font_info{};
        font_info.path = "fonts/OpenSans-Regular.ttf";
        font_info.characters = characters.data();
        font_info.characters_count = characters.size();
        font_info.glyph_resolution = 64;
        Font *font = Resources::createFont(font_info, "font");
        text_pipeline_instance->setTexture("mtsdf", font->getTextureAtlas());


        Entity text_entity = scene->createEntity3D();
        TextRenderer &text_renderer = text_entity.attach<TextRenderer>();

        text_renderer.pipeline_instance = text_pipeline_instance;
        text_renderer.text = TEXT.data();
        text_renderer.text_length = TEXT.length();
        text_renderer.font = font;
        text_renderer.space_width = 0.2f;
        text_renderer.line_height = 1.0f;
        text_renderer.buildMesh();

        MeshInfo quad_mesh_info{};
        quad_mesh_info.attribute_infos=&VERTEX_ATTRIBUTE_INFO_POSITION3D;
        quad_mesh_info.attribute_info_count=1;
        quad_mesh_info.flags=MESH_FLAG_NONE;
        Mesh* quad_mesh = Resources::createMesh(quad_mesh_info,"quad");
        Entity square = scene->createEntity3D();
        MeshRenderer& square_renderer = square.attach<MeshRenderer>();
        square_renderer.mesh=quad_mesh;
        square_renderer.pipelineInstance = background_pipeline_instance;
        square.get<Transform>().translate(vec3(0,0,-0.1));
        vec4 background_color = vec4(1,0,0,1);
        background_pipeline_instance->setUniform("material",&background_color);
        Geometry::createQuad(*square_renderer.mesh,1,1,VERTEX_FLAG_NONE);
    }

};