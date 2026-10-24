

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
        private:
            inline static ClientInstance* instance = nullptr;
    };
}

#endif //CLOUD_CLIENT_CLIENTINSTANCE_H