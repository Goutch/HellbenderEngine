#include "Component.h"
#include "HBE.h"

void Component::subscribeDraw() {
    if (!subscribed_draw) {
        subscribed_draw = true;
        HBE::scene->subscribeDraw(this);
    }
}

void Component::unsubscribeDraw() {
    if (subscribed_draw) {
        subscribed_draw = false;
        HBE::scene->unsubscribeDraw(this);
    }
}

void Component::subscribeUpdate() {
    if (!subscribed_update) {
        subscribed_update = true;
        HBE::scene->subscribeUpdate(this);
    }
}

void Component::unsubscribeUpdate() {
    if (subscribed_update) {
        subscribed_update = false;
        HBE::scene->unsubscribeUpdate(this);
    }
}

void Component::onDetach() {
    if (subscribed_update)unsubscribeUpdate();
    if (subscribed_draw)unsubscribeDraw();
}

