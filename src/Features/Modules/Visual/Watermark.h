//
// Created by Edward on 21/10/2026.
//

#ifndef DLL_TEST_WATERMARK_H
#define DLL_TEST_WATERMARK_H

#include "Features/Modules/Module.h"
#include "Events/Events.h"

class Watermark : public Module {
public:
    Watermark() : Module("Watermark", "shows da watermark") {}

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

#endif //DLL_TEST_WATERMARK_H
