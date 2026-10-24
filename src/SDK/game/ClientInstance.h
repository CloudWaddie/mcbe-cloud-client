

//
// Created by Edward on 16/10/2026.
//

#ifndef CLOUD_CLIENT_CLIENTINSTANCE_H
#define CLOUD_CLIENT_CLIENTINSTANCE_H
#include <optional>
#include <string>

#include "sdk/memory/callvfunc.h"

namespace SDK {
    class ClientInstance {
        public:
            static ClientInstance* get();
            static void setInstance(ClientInstance* newInstance) { instance = newInstance; }
            void* getLocalPlayer() {
                return SDK::Memory::CallVFunc<void*>(this, 31);
            }
            std::string getScreenName() {
                    alignas(std::string) std::byte raw[sizeof(std::string)];
                    auto* gameStr = SDK::Memory::CallVFunc<std::string*, void*>(this, 246, raw);
                    return std::string(gameStr->data(), gameStr->size());
            }
        private:
            inline static ClientInstance* instance = nullptr;
    };
}

#endif //CLOUD_CLIENT_CLIENTINSTANCE_H