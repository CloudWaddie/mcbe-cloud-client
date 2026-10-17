#pragma once

#include <libhat/signature.hpp>

namespace SDK {
    class SigManager {
    public:
        inline static constexpr auto Platform_GameCore = hat::compile_signature<"4C 89 3D ? ? ? ? 4D 85 FF">();
    };
}