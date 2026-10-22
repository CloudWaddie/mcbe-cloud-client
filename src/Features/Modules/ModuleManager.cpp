//
// Created by Edward on 20/10/2026.
//

#include "ModuleManager.h"

#include "Visual/ClickGUI.h"
#include "Visual/DevConsole.h"
#include "Visual/Watermark.h"

void ModuleManager::init () {
    registerModule<Watermark>();
    registerModule<DevConsole>();
    registerModule<ClickGUI>();
    for (auto& m : modules) m->onInit();
}
void ModuleManager::shutdown() {
    for (auto& m : modules) {
        m->setEnabled(false);
    }
    modules.clear();
}

Module* ModuleManager::getModule(const std::string& name) {
    for (auto& m : modules) {
        if (m->getName() == name) return m.get();
    }
    return nullptr;
}

std::vector<Module*> ModuleManager::getModulesByCategory(const Category category) {
    auto modulesForCategory = std::vector<Module*>();
    for (auto& m : modules) {
        if (m->getCategory() == category) modulesForCategory.push_back(m.get());
    }
    return modulesForCategory;
}
