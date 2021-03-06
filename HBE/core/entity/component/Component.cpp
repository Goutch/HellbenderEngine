#include "Component.h"
#include "Application.h"
namespace HBE {
    void Component::subscribeDraw() {
        if (!subscribed_draw) {
            subscribed_draw = true;
            Application::scene->subscribeDraw(this);
        }
    }

    void Component::unsubscribeDraw() {
        if (subscribed_draw) {
            subscribed_draw = false;
            Application::scene->unsubscribeDraw(this);
        }
    }

    void Component::subscribeUpdate() {
        if (!subscribed_update) {
            subscribed_update = true;
            Application::scene->subscribeUpdate(this);
        }
    }

    void Component::unsubscribeUpdate() {
        if (subscribed_update) {
            subscribed_update = false;
            Application::scene->unsubscribeUpdate(this);
        }
    }

    std::string Component::toString() const {
        return "UnamedComponent";
    }

    Component::~Component() {
        if (subscribed_update)unsubscribeUpdate();
        if (subscribed_draw)unsubscribeDraw();
    }
}
