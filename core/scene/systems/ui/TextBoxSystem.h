#pragma once

#include "core/scene/System.h"
#include "core/scene/Entity.h"
#include "Core.h"
#include "HBETypes.h"
#include "core/scene/ecs/Component.h"

using namespace HBE;

namespace HBE {
    class Font;


    struct HB_API TextBoxComponent {
        COMPONENT_IDS(TextBoxComponent)
        friend class TextBoxSystem;

        Entity entity;

        void setSize(vec2 size);

        void setTextHeight(float height);

        void setHintText(const std::string &hint);

    private :
        std::string hint_text;
        bool is_hint_visible = true;
    };

    class HB_API TextBoxSystem : public System {
    private:
        float cursor_blink_t = 0;
        Entity active_textbox_entity;

    public:
        TextBoxSystem(Scene *scene);

        void onAttachTextBox(Entity text_box);

        void onTextBoxClicked(Entity button);

        void onUpdate(float delta);

        void onCharacterPressed(char character);
    };
}
