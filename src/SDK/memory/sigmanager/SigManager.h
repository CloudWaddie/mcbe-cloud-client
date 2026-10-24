#pragma once

#include <libhat/signature.hpp>

namespace SDK {
    class SigManager {
    public:
        inline static constexpr auto ClientInstance_Update = hat::compile_signature<"55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 89 D3 48 89 CE 48 8B 01">();
    };
}