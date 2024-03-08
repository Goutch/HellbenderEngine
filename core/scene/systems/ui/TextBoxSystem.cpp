#include "TextBoxSystem.h"
#include "ButtonSystem.h"
#include "core/scene/Scene.h"
#include "core/input/Input.h"

namespace HBE {

	void TextBoxSystem::onUpdate(float delta) {
		if (active_textbox_entity.valid() && active_textbox_entity.has<TextBoxComponent>()) {
			ButtonComponent *button_component = active_textbox_entity.get<ButtonComponent>();
			cursor_blink_t += delta;
			if (cursor_blink_t > 1.0f) {
				cursor_blink_t = 0;
			}

			if (Input::getKeyDown(KEY_BACKSPACE)) {
				std::string current_text = button_component->getText();
				if (current_text.size() > 0) {
					current_text.pop_back();
					button_component->setText(current_text);
				}
			}
			if (Input::getKeyDown(KEY_ENTER)) {
				active_textbox_entity = Entity::NULL_ENTITY;
			}
		}
	}

	TextBoxSystem::TextBoxSystem(Scene *scene) : System(scene) {
		scene->onAttach<TextBoxComponent>().subscribe(this, &TextBoxSystem::onAttachTextBox);
		scene->onUpdate.subscribe(this, &TextBoxSystem::onUpdate);
		Input::onCharDown.subscribe(this, &TextBoxSystem::onCharacterPressed);
	}

	void TextBoxSystem::onCharacterPressed(char character) {
		if (active_textbox_entity.valid() && active_textbox_entity.has<ButtonComponent>()) {
			ButtonComponent *button_component = active_textbox_entity.get<ButtonComponent>();
			std::string current_text = button_component->getText();
			current_text += (char) character;
			button_component->setText(current_text);
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

		button_component->onButtonClicked.subscribe(this, &TextBoxSystem::onTextBoxClicked);
	}

	void TextBoxSystem::onTextBoxClicked(Entity button) {
		active_textbox_entity = button;
		cursor_blink_t = 0;
	}

	void TextBoxComponent::setSize(vec2 size) {
		ButtonComponent *buttonComponent = entity.get<ButtonComponent>();
		buttonComponent->setSize(size);
	}

	void TextBoxComponent::setTextHeight(float height) {
		ButtonComponent *button_component = entity.get<ButtonComponent>();
		button_component->setTextHeight(height);
	}
}