
#include "LabelSystem.h"
#include "core/scene/systems/MeshRendererSystem.h"
#include "core/resource/Shader.h"
#include "core/resource/Resources.h"
#include "core/resource/RasterizationPipeline.h"
#include "core/resource/RasterizationPipelineInstance.h"
#include "core/scene/components/Transform.h"
#include "core/scene/Scene.h"
#include "core/utility/Geometry.h"

namespace HBE {
	void LabelSystem::onAttachLabel(Entity label) {
		LabelComponent *label_component = label.get<LabelComponent>();

		label_component->pipeline_instance = default_text_pipeline_instance;
		label_component->font = default_font;
		label_component->height = 40;
	}

	LabelSystem::~LabelSystem() {
		delete default_font;
		delete default_text_pipeline_instance;
		delete default_text_pipeline;
		delete default_text_vert_shader;
		delete default_text_frag_shader;
	}

	LabelSystem::LabelSystem(Scene *scene, RasterizationTarget *render_target) : System(scene) {
		scene->onAttach<LabelComponent>().subscribe(this, &LabelSystem::onAttachLabel);
		scene->onDetach<LabelComponent>().subscribe(this, &LabelSystem::onDetachLabel);
		scene->onPrepareRenderGraphOrdered.subscribe(this, &LabelSystem::drawSceneNode);

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

	void LabelSystem::drawSceneNode(RenderGraph *graph, SceneNode &node) {
		if (node.entity.has<LabelComponent>()) {
			LabelComponent *label_component = node.entity.get<LabelComponent>();

			if (!label_component->active ||
			    label_component->mesh == nullptr ||
			    label_component->pipeline_instance == nullptr) {
				return;
			}
			HB_ASSERT(label_component->pipeline_instance != nullptr, "graphic pipeline instance is not set");
			Transform *transform = node.entity.get<Transform>();
			DrawCmdInfo cmd{};
			cmd.mesh = label_component->mesh;
			cmd.pipeline_instance = label_component->pipeline_instance;
			cmd.order_in_layer = node.getHierarchyOrder();
			LabelPushConstant push_constant{};
			push_constant.text_height = label_component->height;
			push_constant.world_matrix = transform->world();

			PushConstantInfo push_constant_info{};
			push_constant_info.name = "constants";
			push_constant_info.size = sizeof(mat4) + sizeof(float);
			push_constant_info.data = &push_constant;
			cmd.push_constants = &push_constant_info;
			cmd.push_constants_count = 1;
			cmd.flags = DRAW_CMD_FLAG_ORDERED;
			cmd.layer = label_component->layer;

			graph->add(cmd);
		}
	}

	void LabelSystem::onDetachLabel(Entity label) {
		LabelComponent *label_component = label.get<LabelComponent>();
		if (label_component->mesh != nullptr) {
			Graphics::waitLastFrame();
			delete label_component->mesh;
			label_component->mesh = nullptr;
		}
	}

	void LabelComponent::setText(const std::string &text) {
		this->text = text;
		HB_ASSERT(font != nullptr, "Font is not set");
		if (text.length() == 0 && mesh != nullptr) {
			Graphics::waitLastFrame();
			delete mesh;
			mesh = nullptr;
			return;
		}
		if (mesh != nullptr) {
			Geometry::updateText(*mesh,
			                     text,
			                     *font,
			                     1.0f,
			                     0.5f,
			                     alignment,
			                     anchor,
			                     size.x,
			                     size.y);
		} else {
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

	const std::string &LabelComponent::getText() const {
		return text;
	}

	vec2 LabelComponent::getSize() const {
		return size;
	}


}

