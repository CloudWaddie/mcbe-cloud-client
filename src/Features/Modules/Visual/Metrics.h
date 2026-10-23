//
// Created by Edward on 23/10/2026.
//

#ifndef CLOUD_CLIENT_METRICS_H
#define CLOUD_CLIENT_METRICS_H

#include "Features/Modules/Module.h"
#include "Events/Events.h"

class Metrics : public Module {
public:
    Metrics() : Module("Metrics", "debug metrics", Category::Visual) {}

    void onInit() override { setEnabled(false); }
    void onEnable() override {
        Events::dispatcher->listen<Events::RenderEvent, &Metrics::onRender>(this);
    }
    void onDisable() override {
        Events::dispatcher->deafen<Events::RenderEvent, &Metrics::onRender>(this);
    }

private:
    void onRender(Events::RenderEvent& event);
};

#endif //CLOUD_CLIENT_METRICS_H
