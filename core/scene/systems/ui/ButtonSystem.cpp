#include "ButtonSystem.h"
#include "UIPanelSystem.h"
#include "core/scene/Scene.h"
#include "core/input/Input.h"
#include "core/resource/Resources.h"
#include "core/utility/Geometry.h"
#include "core/resource/Mesh.h"
#include "core/resource/Font.h"
#include "core/resource/Shader.h"
#include "core/input/Input.h"
#include "core/resource/RasterizationPipeline.h"
#include "core/resource/RasterizationPipelineInstance.h"
#include "core/scene/components/HierarchyNode.h"

namespace HBE {
	ButtonSystem::ButtonSystem(Scene *scene, RasterizationTarget *render_target) : System(scene) {
		scene->onAttach<ButtonComponent>().subscribe(this, &ButtonSystem::onAttachButton);
		scene->onDetach<ButtonComponent>().subscribe(this, &ButtonSystem::onDettachButton);
		Input::onMouseLeftClickDown.subscribe(this, &ButtonSystem::onLeftClick);

		scene->onUpdate.subscribe(this, &ButtonSystem::onUpdate);
		//region ---------------------------------------------button pipelines---------------------------------------------
		ShaderInfo button_frag_shader_info{};
		ShaderInfo button_vert_shader_info{};

		button_vert_shader_info.path = "shaders/defaults/ui/Button.vert";
		button_vert_shader_info.stage = SHADER_STAGE_VERTEX;

		button_frag_shader_info.path = "shaders/defaults/ui/Button.frag";
		button_frag_shader_info.stage = SHADER_STAGE_FRAGMENT;

		button_vert_shader = Resources::createShader(button_vert_shader_info);
		button_frag_shader = Resources::createShader(button_frag_shader_info);

		RasterizationPipelineInfo button_pipeline_info{};
		button_pipeline_info.vertex_shader = button_vert_shader;
		button_pipeline_info.fragment_shader = button_frag_shader;
		button_pipeline_info.rasterization_target = render_target;
		button_pipeline_info.attribute_info_count = 1;
		button_pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
		button_pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NO_DEPTH_TEST;
		button_pipeline = Resources::createRasterizationPipeline(button_pipeline_info);


		RasterizationPipelineInstanceInfo button_pipeline_instance_info{};
		button_pipeline_instance_info.rasterization_pipeline = button_pipeline;
		button_pipeline_instance_info.flags = RASTERIZATION_PIPELINE_INSTANCE_FLAG_NONE;


		default_button_pipeline_instance = Resources::createRasterizationPipelineInstance(button_pipeline_instance_info);
		default_button_hover_pipeline_instance = Resources::createRasterizationPipelineInstance(button_pipeline_instance_info);
		default_button_pressed_pipeline_instance = Resources::createRasterizationPipelineInstance(button_pipeline_instance_info);


		default_button_material = ButtonMaterial{
				DEFAULT_BUTTON_BACKGROUND_COLOR,
				DEFAULT_BUTTON_BORDER_COLOR,
				DEFAULT_BUTTON_BORDER_WIDTH
		};

		default_button_material_hover = ButtonMaterial{
				DEFAULT_BUTTON_HOVER_BACKGROUND_COLOR,
				DEFAULT_BUTTON_HOVER_BORDER_COLOR,
				DEFAULT_BUTTON_BORDER_WIDTH
		};

		default_button_material_pressed = ButtonMaterial{
				DEFAULT_BUTTON_PRESSED_BACKGROUND_COLOR,
				DEFAULT_BUTTON_PRESSED_BORDER_COLOR,
				DEFAULT_BUTTON_BORDER_WIDTH
		};

		default_button_pipeline_instance->setUniform("material", &default_button_material);
		default_button_hover_pipeline_instance->setUniform("material", &default_button_material_hover);
		default_button_pressed_pipeline_instance->setUniform("material", &default_button_material_pressed);

		//endregion
	}

	void ButtonSystem::onAttachButton(Entity entity) {
		ButtonComponent *button = entity.get<ButtonComponent>();
		button->entity = entity;
		button->label_entity = scene->createEntity3D();

		UIPanel *background_renderer = entity.attach<UIPanel>();
		background_renderer->pipeline_instance = default_button_pipeline_instance;
		//background_renderer->active = true;

		button->label_entity = scene->createEntity3D();
		TextComponent *label_component = button->label_entity.attach<TextComponent>();
		label_component->height = 40;

		scene->setParent(button->label_entity, entity);

		background_renderer->size = vec2(50, 50);
	}

	void ButtonSystem::onDettachButton(Entity e) {
		ButtonComponent *button = e.get<ButtonComponent>();
		button->label_entity.destroy();
	}

	void ButtonSystem::onLeftClick(vec2 position) {
		auto group = scene->group<HierarchyNode, Transform, UIPanel, ButtonComponent>();
		for (auto [e, node, t, p, b]: group) {
			if (!node.isActiveInHierarchy())
				continue;

			vec2 min = vec2(t.worldPosition()) - (p.size / 2.0f);
			vec2 max = vec2(t.worldPosition()) + (p.size / 2.0f);
			if (max.x > position.x && min.x < position.x && max.y > position.y && min.y < position.y) {
				b.onButtonClicked.invoke(Entity(e, scene));
			}
		}
	}

	ButtonSystem::~ButtonSystem() {
		delete default_button_pressed_pipeline_instance;
		delete default_button_hover_pipeline_instance;
		delete default_button_pipeline_instance;
		delete button_pipeline;
		delete button_vert_shader;
		delete button_frag_shader;
		scene->onUpdate.unsubscribe(this);
		Input::onMouseLeftClickDown.unsubscribe(this);
	}

	void ButtonSystem::onUpdate(float delta) {
		vec2 position = Input::getMousePosition();
		auto group = scene->group<HierarchyNode, Transform, UIPanel, ButtonComponent>();
		for (auto [e, node, t, p, b]: group) {
			if (!node.isActiveInHierarchy())
				continue;

			vec2 world_position = vec2(t.worldPosition());
			vec2 min = (world_position - (p.size / 2.0f));
			vec2 max = (world_position + (p.size / 2.0f));
			if (max.x > position.x && min.x < position.x && max.y > position.y && min.y < position.y) {
				p.pipeline_instance = default_button_hover_pipeline_instance;
			} else {
				p.pipeline_instance = default_button_pipeline_instance;
			}
		}
	}

	void ButtonComponent::setSize(vec2 size) {
		UIPanel *background_renderer = entity.get<UIPanel>();
		background_renderer->size = size;
	}

	void ButtonComponent::setTextHeight(float height) {
		TextComponent *label_component = label_entity.get<TextComponent>();
		label_component->height = height;
	}

	void ButtonComponent::setText(const std::string &text) {
		TextComponent *label_component = label_entity.get<TextComponent>();
		label_component->setText(text);
	}

	void ButtonComponent::setFont(Font *font) {
		TextComponent *label_component = label_entity.get<TextComponent>();
		label_component->font = font;
	}

	const std::string &ButtonComponent::getText(){
		const TextComponent *label_component = label_entity.get<TextComponent>();
		return label_component->getText();
	}
}

