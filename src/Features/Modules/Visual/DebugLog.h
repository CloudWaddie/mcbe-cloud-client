//
// Created by Edward on 23/10/2026.
//

#ifndef CLOUD_CLIENT_DEBUGLOG_H
#define CLOUD_CLIENT_DEBUGLOG_H

#include "Features/Modules/Module.h"
#include "Events/Events.h"

class DebugLog : public Module {
public:
    DebugLog() : Module("Debug Log", "debug log", Category::Visual) {}

    void onInit() override { setEnabled(false); }
    void onEnable() override {
        Events::dispatcher->listen<Events::RenderEvent, &DebugLog::onRender>(this);
    }
    void onDisable() override {
        Events::dispatcher->deafen<Events::RenderEvent, &DebugLog::onRender>(this);
    }

private:
    void onRender(Events::RenderEvent& event);
};

#endif //CLOUD_CLIENT_DEBUGLOG_H
