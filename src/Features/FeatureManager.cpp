//
// Created by Edward on 20/10/2026.
//

#include "FeatureManager.h"
#include <nes/event_dispatcher.hpp>

#include "Events/Events.h"
#include "Hooks/CIUpdate.h"
#include "Modules/ModuleManager.h"

namespace FeatureManager {
    void init () {
        Events::dispatcher = std::make_unique<nes::event_dispatcher>();
        Hooks::CIUpdate::init();
        ModuleManager::init();
    }
    void shutdown () {
        Events::dispatcher = nullptr;
        ModuleManager::shutdown();
    }
}
