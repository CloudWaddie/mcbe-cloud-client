

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
            void* getLocalPlayer() {
                return SDK::Memory::CallVFunc<void*>(this, 31);
            }
    };
}

#endif //DLL_TEST_CLIENTINSTANCE_H