//
// Created by Edward on 21/10/2026.
//

#ifndef CLOUD_CLIENT_WATERMARK_H
#define CLOUD_CLIENT_WATERMARK_H

#include "Features/Modules/Module.h"
#include "Events/Events.h"

class Watermark : public Module {
public:
    Watermark() : Module("Watermark", "shows da watermark", Category::Visual) {}

    void onInit() override { setEnabled(true); }
    void onEnable() override {
        Events::dispatcher->listen<Events::RenderEvent, &Watermark::onRender>(this);
    }
    void onDisable() override {
        Events::dispatcher->deafen<Events::RenderEvent, &Watermark::onRender>(this);
    }

private:
    void onRender(Events::RenderEvent& event);
};

#endif //CLOUD_CLIENT_WATERMARK_H
