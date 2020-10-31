#include "Component.h"
#include "HBE.h"

void Component::subscribeDraw() {
    if (!subscribed_draw) {
        subscribed_draw = true;
        HBE::current_scene->subscribeDraw(this);
    }
}

void Component::unsubscribeDraw() {
    if (subscribed_draw) {
        subscribed_draw = false;
        HBE::current_scene->unsubscribeDraw(this);
    }
}

void Component::subscribeUpdate() {
    if (!subscribed_update) {
        subscribed_update = true;
        HBE::current_scene->subscribeUpdate(this);
    }
}

void Component::unsubscribeUpdate() {
    if (subscribed_update) {
        subscribed_update = false;
        HBE::current_scene->unsubscribeUpdate(this);
    }
}

void Component::onDetach() {
    if (subscribed_update)unsubscribeUpdate();
    if (subscribed_draw)unsubscribeDraw();
}
