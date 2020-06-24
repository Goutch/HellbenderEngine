#include "Component.h"
#include "core/PrototypeEngine.h"
#include "core/entity/Scene.h"

void Component::subscribeDraw() {
    if (!subscribed_draw) {
        subscribed_draw = true;
        PrototypeEngine::current_scene->subscribeDraw(this);
    }

}

void Component::unsubscribeDraw() {
    if (subscribed_draw) {
        subscribed_draw = false;
        PrototypeEngine::current_scene->unsubscribeDraw(this);
    }
}

void Component::subscribeUpdate() {
    if (!subscribed_update) {
        subscribed_update = true;
        PrototypeEngine::current_scene->subscribeUpdate(this);
    }
}

void Component::unsubscribeUpdate() {
    if (subscribed_update) {
        subscribed_update = false;
        PrototypeEngine::current_scene->unsubscribeUpdate(this);
    }
}

void Component::onDetach() {
    if (subscribed_update)unsubscribeUpdate();
    if (subscribed_draw)unsubscribeDraw();
}
