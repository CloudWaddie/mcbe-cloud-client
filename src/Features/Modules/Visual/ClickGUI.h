#ifndef CLOUD_CLIENT_CLICKGUI_H
#define CLOUD_CLIENT_CLICKGUI_H

#include "Globals.h"
#include "Features/Modules/Module.h"
#include "Events/Events.h"

class ClickGUI : public Module {
public:
    char notificationText[256] = "";
    ClickGUI() : Module("ClickGUI", "GUI for Clicking", Category::Visual) {}

    void onInit() override {
        setKeybind(75); // letter K
        setEnabled(true);
    }
    void onEnable() override {
        Client::clickGuiOpen = true;
        Events::dispatcher->listen<Events::RenderEvent, &ClickGUI::onRender>(this);
    }
    void onDisable() override {
        Client::clickGuiOpen = false;
        Events::dispatcher->deafen<Events::RenderEvent, &ClickGUI::onRender>(this);
    }

private:
    void onRender(Events::RenderEvent& event);
};

#endif //CLOUD_CLIENT_CLICKGUI_H
