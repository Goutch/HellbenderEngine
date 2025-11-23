#include "TextSystem.h"
#include "core/scene/systems/MeshRendererSystem.h"
#include "core/resource/Shader.h"
#include "core/resource/Resources.h"
#include "core/resource/RasterizationPipeline.h"
#include "core/resource/RasterizationPipelineInstance.h"
#include "core/scene/components/Transform.h"
#include "core/scene/Scene.h"
#include "core/scene/components/Node3D.h"
#include "core/utility/Geometry.h"

namespace HBE
{
	void TextSystem::onAttachLabel(Entity text_entity)
	{
		TextComponent* text_component = text_entity.get<TextComponent>();

		text_component->pipeline_instance = default_text_pipeline_instance;
		text_component->font = default_font;
		text_component->height = 40;
	}

	TextSystem::~TextSystem()
	{
		delete default_font;
		delete default_text_pipeline_instance;
		delete default_text_pipeline;
		delete default_text_vert_shader;
		delete default_text_frag_shader;
	}

	TextSystem::TextSystem(Scene* scene, RasterizationTarget* render_target) : System(scene)
	{
		scene->onAttach<TextComponent>().subscribe(this, &TextSystem::onAttachLabel);
		scene->onDetach<TextComponent>().subscribe(this, &TextSystem::onDetachLabel);
		std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
		FontInfo font_info{};
		font_info.path = "fonts/Roboto-Regular.ttf";
		font_info.characters = characters.data();
		font_info.characters_count = characters.size();
		font_info.glyph_resolution = 64;

		default_font = Resources::createFont(font_info);

		ShaderInfo text_frag_shader_info{};
		ShaderInfo text_vert_shader_info{};

		text_vert_shader_info.path = "shaders/defaults/ui/Label.vert";
		text_vert_shader_info.stage = SHADER_STAGE_VERTEX;

		text_frag_shader_info.path = "shaders/defaults/ui/Label.frag";
		text_frag_shader_info.stage = SHADER_STAGE_FRAGMENT;


		default_text_vert_shader = Resources::createShader(text_vert_shader_info);
		default_text_frag_shader = Resources::createShader(text_frag_shader_info);


		RasterizationPipelineInfo text_pipeline_info{};
		text_pipeline_info.vertex_shader = default_text_vert_shader;
		text_pipeline_info.fragment_shader = default_text_frag_shader;
		text_pipeline_info.rasterization_target = render_target;
		text_pipeline_info.attribute_info_count = 1;
		text_pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
		text_pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NO_DEPTH_TEST;
		default_text_pipeline = Resources::createRasterizationPipeline(text_pipeline_info);

		RasterizationPipelineInstanceInfo text_pipeline_instance_info{};
		text_pipeline_instance_info.rasterization_pipeline = default_text_pipeline;
		text_pipeline_instance_info.flags = RASTERIZATION_PIPELINE_INSTANCE_FLAG_NONE;
		default_text_pipeline_instance = Resources::createRasterizationPipelineInstance(text_pipeline_instance_info);

		default_text_pipeline_instance->setUniform("material", &DEFAULT_TEXT_COLOR);
		default_text_pipeline_instance->setImage("mtsdf", default_font->getTextureAtlas());
	}

	void TextSystem::onDetachLabel(Entity label)
	{
		TextComponent* label_component = label.get<TextComponent>();
		if (label_component->mesh != nullptr)
		{
			Graphics::waitLastFrame();
			delete label_component->mesh;
			label_component->mesh = nullptr;
		}
	}

	void TextSystem::onPrepareRenderGraph(RenderGraph* render_graph)
	{
		HB_PROFILE_BEGIN("TextComponentDraw");
		auto group = scene->group<Node3D, Transform, TextComponent>();

		for (auto [handle,node, transform, text_component] : group)
		{
			if (!text_component.active ||
				text_component.mesh == nullptr ||
				text_component.pipeline_instance == nullptr ||
				node.isActiveInHierarchy())
			{
				continue;
			}
			HB_ASSERT(text_component.pipeline_instance != nullptr, "graphic pipeline instance is not set");

			DrawCmdInfo cmd{};
			cmd.mesh = text_component.mesh;
			cmd.pipeline_instance = text_component.pipeline_instance;
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

			render_graph->add(cmd);
		}
		HB_PROFILE_END("TextComponentDraw");
	}

	void TextComponent::setText(const std::string& text)
	{
		this->text = text;
		HB_ASSERT(font != nullptr, "Font is not set");
		if (text.length() == 0 && mesh != nullptr)
		{
			Graphics::waitLastFrame();
			delete mesh;
			mesh = nullptr;
			return;
		}
		if (mesh != nullptr)
		{
			Geometry::updateText(*mesh,
			                     text,
			                     *font,
			                     1.0f,
			                     0.5f,
			                     alignment,
			                     anchor,
			                     size.x,
			                     size.y);
		}
		else
		{
			mesh = Geometry::createText(text,
			                            *font,
			                            1.0f,
			                            0.5f,
			                            alignment,
			                            anchor,
			                            size.x,
			                            size.y);
		}
	}

	const std::string& TextComponent::getText() const
	{
		return text;
	}

	vec2 TextComponent::getSize() const
	{
		return size;
	}
}
