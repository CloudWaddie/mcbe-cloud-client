#ifndef CLOUD_CLIENT_CALLVFUNC_H
#define CLOUD_CLIENT_CALLVFUNC_H
#include <cstddef>

namespace SDK {
    namespace Memory {
        template<typename ReturnType, typename class_t, typename... Args>
        ReturnType CallVFunc(class_t* pThis, std::size_t index, Args... args) {
            void** vtable = *reinterpret_cast<void***>(pThis);
            using Fn = ReturnType (__thiscall class_t::*)(Args...);
            Fn func = *reinterpret_cast<Fn*>(&vtable[index]);
            return (pThis->*func)(args...);
        }
    }
}

#endif //CLOUD_CLIENT_CALLVFUNC_H