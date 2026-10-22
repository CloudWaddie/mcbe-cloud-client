//
// Created by Edward on 21/10/2026.
//

#ifndef CLOUD_CLIENT_DEVCONSOLE_H
#define CLOUD_CLIENT_DEVCONSOLE_H

#include "Features/Modules/Module.h"
#include "Events/Events.h"

class DevConsole : public Module {
public:
    char notificationText[256] = "";
    DevConsole() : Module("Dev Console", "dev screen idk what ima put here", Category::Visual) {}

    void onInit() override { setEnabled(true); }
    void onEnable() override {
        Events::dispatcher->listen<Events::RenderEvent, &DevConsole::onRender>(this);
    }
    void onDisable() override {
        Events::dispatcher->deafen<Events::RenderEvent, &DevConsole::onRender>(this);
    }

private:
    void onRender(Events::RenderEvent& event);
};

#endif //CLOUD_CLIENT_DEVCONSOLE_H
