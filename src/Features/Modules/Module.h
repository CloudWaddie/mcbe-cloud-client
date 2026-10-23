//
// Created by Edward on 20/10/2026.
//

#ifndef CLOUD_CLIENT_MODULE_H
#define CLOUD_CLIENT_MODULE_H
#include <atomic>
#include <string>
#include <utility>

#include "Category.h"

class Module {
    public:
        Module(std::string name, std::string description, const Category category) : name(std::move(name)), description(std::move(description)), category(category) {}
        virtual ~Module() = default;
        virtual void onEnable() {}
        virtual void onDisable() {}
        virtual void onTick() {}
        virtual void onInit() {}
        Category getCategory() const { return category; }
        const std::string& getName() const { return name; }
        const std::string& getDescription() const { return description; }
        bool isEnabled() const { return enabled; }
        void setEnabled(bool const value) {
                if (enabled.exchange(value) == value) return;
                if (value) onEnable();
                else onDisable();
        }

        void toggle() { setEnabled(!enabled); }
    private:
        std::string name;
        std::string description;
        std::atomic<bool> enabled = false;
        Category category;
};

#endif //CLOUD_CLIENT_MODULE_H
