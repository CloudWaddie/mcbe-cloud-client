//
// Created by Edward on 20/10/2026.
//

#ifndef DLL_TEST_MODULEMANAGER_H
#define DLL_TEST_MODULEMANAGER_H
#include <memory>
#include <string>
#include <vector>
#include "Module.h"

class ModuleManager {
public:
    static void init();
    static void shutdown();

    static Module* getModule(const std::string& name);

    template <class T>
    static T* getModule() {
        for (auto& m : modules) {
            if (auto* t = dynamic_cast<T*>(m.get())) return t;
        }
        return nullptr;
    }

    static const std::vector<std::unique_ptr<Module>>& getModules() { return modules; }
private:
    template <class T>
    static void registerModule() { modules.push_back(std::make_unique<T>()); }

    inline static std::vector<std::unique_ptr<Module>> modules;
};

#endif //DLL_TEST_MODULEMANAGER_H
