//
// Created by Edward on 18/10/2026.
//

#ifndef DLL_TEST_CALLVFUNC_H
#define DLL_TEST_CALLVFUNC_H
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

#endif //DLL_TEST_CALLVFUNC_H
