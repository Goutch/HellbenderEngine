#pragma once
#include "core/utility/Event.h"
class Configs{
    static bool vertical_sync;

public:
    static Event<bool> OnVerticalSyncChange;
    static void setVerticalSync(bool v_sync);
    static bool getVerticalSync();
};
