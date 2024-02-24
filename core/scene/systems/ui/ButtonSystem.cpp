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
#include "core/resource/GraphicPipeline.h"
#include "core/resource/GraphicPipelineInstance.h"

namespace HBE {
	ButtonSystem::ButtonSystem(Scene *scene, RenderTarget *render_target) : System(scene) {
		scene->onAttach<ButtonComponent>().subscribe(this, &ButtonSystem::onAttachButton);
		scene->onDetach<ButtonComponent>().subscribe(this, &ButtonSystem::onDettachButton);
		Input::onMouseLeftClickDown.subscribe(this, &ButtonSystem::onLeftClick);

		scene->onUpdate.subscribe(this, &ButtonSystem::onUpdate);
		//region ---------------------------------------------button pipelines---------------------------------------------
		const char *characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
		FontInfo font_info{};
		font_info.path = "fonts/OpenSans-Regular.ttf";
		font_info.characters = characters;
		font_info.characters_count = 94;
		font_info.glyph_resolution = 64;

		default_font = Resources::createFont(font_info);

		ShaderInfo button_frag_shader_info{};
		ShaderInfo button_vert_shader_info{};
		ShaderInfo text_frag_shader_info{};
		ShaderInfo text_vert_shader_info{};

		button_vert_shader_info.path = "shaders/defaults/ui/Button.vert";
		button_vert_shader_info.stage = SHADER_STAGE_VERTEX;

		button_frag_shader_info.path = "shaders/defaults/ui/Button.frag";
		button_frag_shader_info.stage = SHADER_STAGE_FRAGMENT;

		text_vert_shader_info.path = "shaders/defaults/TextMSDF.vert";
		text_vert_shader_info.stage = SHADER_STAGE_VERTEX;

		text_frag_shader_info.path = "shaders/defaults/TextMSDF.frag";
		text_frag_shader_info.stage = SHADER_STAGE_FRAGMENT;

		button_vert_shader = Resources::createShader(button_vert_shader_info);
		button_frag_shader = Resources::createShader(button_frag_shader_info);

		default_text_vert_shader = Resources::createShader(text_vert_shader_info);
		default_text_frag_shader = Resources::createShader(text_frag_shader_info);

		GraphicPipelineInfo button_pipeline_info{};
		button_pipeline_info.vertex_shader = button_vert_shader;
		button_pipeline_info.fragment_shader = button_frag_shader;
		button_pipeline_info.render_target = render_target;
		button_pipeline_info.attribute_info_count = 1;
		button_pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
		button_pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NO_DEPTH_TEST;
		button_pipeline = Resources::createGraphicPipeline(button_pipeline_info);

		GraphicPipelineInfo default_text_pipeline_info{};
		default_text_pipeline_info.vertex_shader = default_text_vert_shader;
		default_text_pipeline_info.fragment_shader = default_text_frag_shader;
		default_text_pipeline_info.render_target = render_target;
		default_text_pipeline_info.attribute_info_count = 1;
		default_text_pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
		default_text_pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NO_DEPTH_TEST;
		default_text_pipeline = Resources::createGraphicPipeline(default_text_pipeline_info);

		GraphicPipelineInstanceInfo button_pipeline_instance_info{};
		button_pipeline_instance_info.graphic_pipeline = button_pipeline;
		button_pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

		GraphicPipelineInstanceInfo text_pipeline_instance_info{};
		text_pipeline_instance_info.graphic_pipeline = default_text_pipeline;
		text_pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

		default_button_pipeline_instance = Resources::createGraphicPipelineInstance(button_pipeline_instance_info);
		default_button_hover_pipeline_instance = Resources::createGraphicPipelineInstance(button_pipeline_instance_info);
		default_button_pressed_pipeline_instance = Resources::createGraphicPipelineInstance(button_pipeline_instance_info);

		default_text_pipeline_instance = Resources::createGraphicPipelineInstance(text_pipeline_instance_info);


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
		default_text_pipeline_instance->setUniform("material", &DEFAULT_TEXT_COLOR);
		default_text_pipeline_instance->setTexture("mtsdf", default_font->getTextureAtlas());

		//endregion
	}

	void ButtonSystem::onAttachButton(Entity entity) {
		ButtonComponent &button = entity.get<ButtonComponent>();
		button.root = entity;
		button.background_entity = scene->createEntity3D();
		button.text_entity = scene->createEntity3D();

		button.background_entity = scene->createEntity3D();
		UIPanel &background_renderer = button.background_entity.attach<UIPanel>();
		background_renderer.pipeline_instance = default_button_pipeline_instance;
		background_renderer.active = true;

		button.text_entity = scene->createEntity3D();
		MeshRenderer &text_renderer = button.text_entity.attach<MeshRenderer>();
		text_renderer.pipeline_instance = default_text_pipeline_instance;
		text_renderer.ordered = true;

		Transform &text_t = button.text_entity.get<Transform>();
		text_t.setPosition(vec3(0, -100, 0));

		scene->setParent(button.background_entity, entity);
		scene->setParent(button.text_entity, button.background_entity);

		button.setSize(vec2(50, 50));
		button.setText("Button", default_font);
		button.setTextHeight(10.0f);
	}

	void ButtonSystem::onDettachButton(Entity e) {
		ButtonComponent &button = e.get<ButtonComponent>();
		if (button.text_mesh != nullptr) {
			delete button.text_mesh;
		}
		button.text_entity.destroy();
		button.background_entity.destroy();

	}

	void ButtonSystem::onLeftClick(vec2 position) {
		auto group = scene->group<EntityState, Transform, ButtonComponent>();
		for (auto [e, s, t, b]: group) {
			if (!scene->isActiveInHierarchy(e))
				continue;
			Transform &background_transform = b.background_entity.get<Transform>();
			vec2 min = (t.worldPosition() - (background_transform.worldScale() / 2.0f));
			vec2 max = (t.worldPosition() + (background_transform.worldScale() / 2.0f));
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
		delete default_font;
		delete default_text_pipeline_instance;
		delete default_text_pipeline;
		delete default_text_vert_shader;
		delete default_text_frag_shader;
		delete button_vert_shader;
		delete button_frag_shader;
		scene->onUpdate.unsubscribe(this);
		Input::onMouseLeftClickDown.unsubscribe(this);
	}

	void ButtonSystem::onUpdate(float delta) {
		vec2 position = Input::getMousePosition();
		auto group = scene->group<EntityState, Transform, ButtonComponent>();
		for (auto [e, s, t, b]: group) {
			if (!scene->isActiveInHierarchy(e))
				continue;
			UIPanel &background_renderer = b.background_entity.get<UIPanel>();

			vec2 world_position = vec2(t.worldPosition());
			vec2 min = (world_position - (background_renderer.size / 2.0f));
			vec2 max = (world_position + (background_renderer.size / 2.0f));
			if (max.x > position.x && min.x < position.x && max.y > position.y && min.y < position.y) {
				background_renderer.pipeline_instance = default_button_hover_pipeline_instance;
			} else {
				background_renderer.pipeline_instance = default_button_pipeline_instance;
			}
		}
	}

	void ButtonComponent::setSize(vec2 size) {
		UIPanel &background_renderer = background_entity.get<UIPanel>();
		background_renderer.size = size;
	}

	void ButtonComponent::setText(const std::string &text, Font *font) {
		MeshRenderer &mesh_renderer = text_entity.get<MeshRenderer>();
		float total_width, total_height;
		if (text_mesh != nullptr) {
			Geometry::updateText(*text_mesh,
			                     text,
			                     *font,
			                     1.0f,
			                     0.5f,
			                     TEXT_ALIGNMENT_CENTER,
			                     PIVOT_CENTER,
			                     total_width,
			                     total_height);
		} else {
			text_mesh = Geometry::createText(text,
			                                 *font,
			                                 1.0f,
			                                 0.5f,
			                                 TEXT_ALIGNMENT_CENTER,
			                                 PIVOT_CENTER,
			                                 total_width,
			                                 total_height);
		}
		mesh_renderer.mesh = text_mesh;
	}

	void ButtonComponent::setTextHeight(float height) {
		Transform &text_transform = text_entity.get<Transform>();
		text_transform.setLocalScale(vec3(height, height, 1.0f));
	}
}

