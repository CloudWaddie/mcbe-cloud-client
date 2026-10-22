//
// Created by Edward on 18/10/2026.
//

#ifndef CLOUD_CLIENT_CALLVFUNC_H
#define CLOUD_CLIENT_CALLVFUNC_H
#include <cstddef>

namespace SDK {
    namespace Memory {
        template<typename ReturnType, typename... Args>
        ReturnType CallVFunc(void* pThis, std::size_t index, Args... args) {
            void** vtable = *reinterpret_cast<void***>(pThis);
            void* func = vtable[index];
            using Fn = ReturnType(__thiscall*)(void*, Args...);
            return reinterpret_cast<Fn>(func)(pThis, args...);
        }
    }
}

#endif //CLOUD_CLIENT_CALLVFUNC_H
