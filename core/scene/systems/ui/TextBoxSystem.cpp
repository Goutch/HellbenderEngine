#include "TextBoxSystem.h"
#include "ButtonSystem.h"
#include "core/scene/Scene.h"
#include "core/input/Input.h"

namespace HBE {

	void TextBoxSystem::onUpdate(float delta) {
		if (active_textbox_entity.valid() && active_textbox_entity.has<TextBoxComponent>()) {
			TextBoxComponent *textbox_component = active_textbox_entity.get<TextBoxComponent>();
			ButtonComponent *button_component = active_textbox_entity.get<ButtonComponent>();
			cursor_blink_t += delta;
			if (cursor_blink_t > 1.0f) {
				cursor_blink_t = 0;
			}

			if (Application::instance->getInput()->getKeyDown(KEY_BACKSPACE)) {
				std::string current_text = button_component->getText();
				if (!current_text.empty()) {
					current_text.pop_back();
					button_component->setText(current_text);
				}
			}
			if (Application::instance->getInput()->getKeyDown(KEY_ENTER)) {
				active_textbox_entity = Entity::NULL_ENTITY;
				if (button_component->getText().empty()) {
					button_component->setText(textbox_component->hint_text);
					textbox_component->is_hint_visible = true;
				}
			}
		}
	}

	TextBoxSystem::TextBoxSystem(Scene *scene) : System(scene) {
		scene->onAttach<TextBoxComponent>().subscribe(on_attach_sub_id, this, &TextBoxSystem::onAttachTextBox);
		scene->onDetach<TextBoxComponent>().subscribe(on_detach_sub_id, this, &TextBoxSystem::onDetachTextBox);
		scene->onUpdate.subscribe(on_update_sub_id, this, &TextBoxSystem::onUpdate);
		Application::instance->getInput()->onCharDown.subscribe(on_char_down_sub_id, this, &TextBoxSystem::onCharacterPressed);
	}

	void TextBoxSystem::onCharacterPressed(char character) {
		if (active_textbox_entity.valid() && active_textbox_entity.has<ButtonComponent>()) {
			TextBoxComponent *textbox_component = active_textbox_entity.get<TextBoxComponent>();
			ButtonComponent *button_component = active_textbox_entity.get<ButtonComponent>();
			std::string current_text = button_component->getText();
			current_text += (char) character;
			button_component->setText(current_text);
			textbox_component->is_hint_visible = false;
		}
	}

	void TextBoxSystem::onAttachTextBox(Entity text_box) {
		ButtonComponent *button_component = nullptr;
		if (!text_box.has<ButtonComponent>()) {
			button_component = text_box.attach<ButtonComponent>();
		} else {
			button_component = text_box.get<ButtonComponent>();
		}
		text_box.get<TextBoxComponent>()->entity = text_box;

		button_component->onButtonClicked.subscribe(button_component->system_clicked_subscription_id, this, &TextBoxSystem::onTextBoxClicked);
	}

	void TextBoxSystem::onDetachTextBox(Entity text_box) {
		ButtonComponent *button_component = text_box.get<ButtonComponent>();
		button_component->onButtonClicked.unsubscribe(button_component->system_clicked_subscription_id);
	}

	void TextBoxSystem::onTextBoxClicked(Entity button) {
		if (active_textbox_entity.valid()) {
			ButtonComponent *button_component = active_textbox_entity.get<ButtonComponent>();
			TextBoxComponent *textbox_component = active_textbox_entity.get<TextBoxComponent>();
			if (button_component->getText().empty()) {
				button_component->setText(textbox_component->hint_text);
				textbox_component->is_hint_visible = true;
			}
		}
		active_textbox_entity = button;
		if (active_textbox_entity.valid()) {
			ButtonComponent *button_component = active_textbox_entity.get<ButtonComponent>();
			TextBoxComponent *textbox_component = active_textbox_entity.get<TextBoxComponent>();
			if (textbox_component->is_hint_visible) {
				button_component->setText("");
				textbox_component->is_hint_visible = false;
			}
		}

		cursor_blink_t = 0;
	}

	TextBoxSystem::~TextBoxSystem() {
		scene->onAttach<TextBoxComponent>().unsubscribe(on_attach_sub_id);
		scene->onUpdate.unsubscribe(on_update_sub_id);
		Application::instance->getInput()->onCharDown.unsubscribe(on_char_down_sub_id);
	}


	void TextBoxComponent::setSize(vec2 size) {
		ButtonComponent *buttonComponent = entity.get<ButtonComponent>();
		buttonComponent->setSize(size);
	}

	void TextBoxComponent::setTextHeight(float height) {
		ButtonComponent *button_component = entity.get<ButtonComponent>();
		button_component->setTextHeight(height);
	}

	void TextBoxComponent::setHintText(const std::string &hint) {
		hint_text = hint;
		ButtonComponent *button_component = entity.get<ButtonComponent>();
		TextBoxComponent *textbox_component = entity.get<TextBoxComponent>();
		if (button_component->getText().empty()) {
			button_component->setText(hint);
			textbox_component->is_hint_visible = true;
		}

	}
}