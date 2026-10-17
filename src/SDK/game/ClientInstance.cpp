//
// Created by Edward on 16/10/2026.
//
#include "ClientInstance.h"
#include "Platform_GameCore.h"
#include "MinecraftGame.h"

SDK::ClientInstance* SDK::ClientInstance::get() {
    auto* platform = Platform_GameCore::get();
    if (!platform)
        return nullptr;

    auto* mcgame = platform->getMinecraftGame();
    if (!mcgame)
        return nullptr;

    return mcgame->getPrimaryClientInstance();
}