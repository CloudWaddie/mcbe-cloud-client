//
// Created by Edward on 16/10/2026.
//

#ifndef CLOUD_CLIENT_PLATFORM_GAMECORE_H
#define CLOUD_CLIENT_PLATFORM_GAMECORE_H

#include "Platform_GameCore.h"
#include "MinecraftGame.h"


namespace SDK {
    class Platform_GameCore {
    public:
        MinecraftGame* getMinecraftGame();
        Platform_GameCore* getGameCore();

        static Platform_GameCore* get();
    };
}

#endif //CLOUD_CLIENT_PLATFORM_GAMECORE_H
