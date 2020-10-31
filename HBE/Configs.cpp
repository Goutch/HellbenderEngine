#include "Configs.h"

bool Configs::vertical_sync=true;

bool Configs::getVerticalSync(){
    return vertical_sync;
}
void Configs::setVerticalSync(bool v_sync){
    vertical_sync = v_sync;
    OnVerticalSyncChange.invoke(v_sync);
}
