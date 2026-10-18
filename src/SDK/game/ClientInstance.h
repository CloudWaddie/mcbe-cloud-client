

//
// Created by Edward on 16/10/2026.
//

#ifndef DLL_TEST_CLIENTINSTANCE_H
#define DLL_TEST_CLIENTINSTANCE_H
#include <optional>
#include <string>

#include "sdk/memory/callvfunc.h"

namespace SDK {
    class ClientInstance {
        public:
            static ClientInstance* get();
            void displayClientMessage(const std::string& message, std::optional<std::string> filtered = std::nullopt) {
                return SDK::Memory::CallVFunc<void, const std::string&, std::optional<std::string>>(this, 0, message, filtered); // TODO: i need to get the right index
            }
    };
}

#endif //DLL_TEST_CLIENTINSTANCE_H