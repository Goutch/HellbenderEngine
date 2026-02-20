
#pragma once

#include "HBETypes.h"
#include "core/scene/Entity.h"
#include "core/scene/System.h"
#include "dependencies/utils-collection/Event.h"
#include "core/scene/ecs/Component.h"

namespace HBE {

	class RasterizationTarget;

	class RasterizationPipelineInstance;

	class RasterizationPipeline;

	class Shader;

	class Mesh;

	class Font;

	struct ButtonMaterial {
		vec4 background_color;
		vec4 border_color;
		float borderWidth;
	};


	struct HB_API ButtonComponent {
		COMPONENT_IDS(ButtonComponent)
		Entity entity;
		Entity label_entity;
		ButtonMaterial material_properties;
		Event<Entity> onButtonClicked;

		void setSize(vec2 size);
		void setText(const std::string &text);
		void setFont(Font *font);
		const std::string& getText();
		void setTextHeight(float height);
	};

	class HB_API ButtonSystem : public System {
		const vec4 DEFAULT_BUTTON_BACKGROUND_COLOR = vec4(0.0, 0.0, 0.0, 1);
		const vec4 DEFAULT_BUTTON_BORDER_COLOR = vec4(1, 1, 1, 1);

		const vec4 DEFAULT_BUTTON_HOVER_BACKGROUND_COLOR = vec4(0.2, 0.2, 0.2, 1);
		const vec4 DEFAULT_BUTTON_HOVER_BORDER_COLOR = vec4(1, 1, 1, 1);

		const vec4 DEFAULT_BUTTON_PRESSED_BACKGROUND_COLOR = vec4(0, 0, 0, 1);
		const vec4 DEFAULT_BUTTON_PRESSED_BORDER_COLOR = vec4(1, 1, 1, 1);

		const float DEFAULT_BUTTON_BORDER_WIDTH = 3;

		const vec4 DEFAULT_TEXT_COLOR = vec4(1, 1, 1, 1);
		event_subscription_id update_subscription_id;
		event_subscription_id left_click_subscription_id;
		event_subscription_id attach_button_subscription_id;
		event_subscription_id detach_button_subscription_id;
		ButtonMaterial default_button_material;
		ButtonMaterial default_button_material_hover;
		ButtonMaterial default_button_material_pressed;

		Shader *button_frag_shader = nullptr;
		Shader *button_vert_shader = nullptr;
		RasterizationPipeline *button_pipeline = nullptr;
		RasterizationPipelineInstance *default_button_pipeline_instance = nullptr;
		RasterizationPipelineInstance *default_button_hover_pipeline_instance = nullptr;
		RasterizationPipelineInstance *default_button_pressed_pipeline_instance = nullptr;

	public:

		ButtonSystem(Scene *scene, RasterizationTarget *render_target);

		~ButtonSystem();


	private:
		void onUpdate(float delta);

		void onAttachButton(Entity e);

		void onDettachButton(Entity e);

		void onLeftClick(vec2 position);



	};
}