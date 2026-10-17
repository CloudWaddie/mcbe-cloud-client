#include <cstdint>
#include <cstring>
#include <libhat/scanner.hpp>
#include "SDK/memory/sigmanager/SigManager.h"
#include "Platform_GameCore.h"
#include <libhat/access.hpp>

SDK::Platform_GameCore* SDK::Platform_GameCore::get() {
    auto result = hat::find_pattern(
        SDK::SigManager::Platform_GameCore,
        ".text"
    );

    auto* address = result.get();

    if (!address)
        return nullptr;

    std::int32_t displacement;
    std::memcpy(&displacement, address + 3, sizeof(displacement));

    auto target = reinterpret_cast<std::uintptr_t>(address)
                + 7 + static_cast<std::intptr_t>(displacement);

    return *reinterpret_cast<SDK::Platform_GameCore**>(target);
}

SDK::MinecraftGame* SDK::Platform_GameCore::getMinecraftGame() {
    const auto gameHolder = hat::member_at<void*>(this, 0x118);
    return gameHolder ? hat::member_at<MinecraftGame*>(gameHolder, 0x48) : nullptr;
}

SDK::Platform_GameCore* SDK::Platform_GameCore::getGameCore() {
    return hat::member_at<Platform_GameCore*>(this, 0x78);
}