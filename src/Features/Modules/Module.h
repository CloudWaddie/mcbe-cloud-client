//
// Created by Edward on 20/10/2026.
//

#ifndef DLL_TEST_MODULE_H
#define DLL_TEST_MODULE_H
#include <atomic>
#include <string>

class Module {
    public:
        Module(std::string name, std::string description) : name(std::move(name)), description(std::move(description)) {}
        virtual ~Module() = default;
        virtual void onEnable() {}
        virtual void onDisable() {}
        virtual void onTick() {}
        virtual void onInit() {}
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
};

#endif //DLL_TEST_MODULE_H
