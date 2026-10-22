#ifndef CLOUD_CLIENT_CLICKGUI_H
#define CLOUD_CLIENT_CLICKGUI_H

#include "Features/Modules/Module.h"
#include "Events/Events.h"

class ClickGUI : public Module {
public:
    char notificationText[256] = "";
    ClickGUI() : Module("ClickGUI", "GUI for Clicking", Category::Visual) {}

    void onInit() override { setEnabled(true); }
    void onEnable() override {
        Events::dispatcher->listen<Events::RenderEvent, &ClickGUI::onRender>(this);
    }
    void onDisable() override {
        Events::dispatcher->deafen<Events::RenderEvent, &ClickGUI::onRender>(this);
    }

private:
    void onRender(Events::RenderEvent& event);
};

#endif //CLOUD_CLIENT_CLICKGUI_H
