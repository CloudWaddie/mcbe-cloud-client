//
// Created by Edward on 16/10/2026.
//
#include <libhat/access.hpp>
#include "MinecraftGame.h"
#include "ClientInstance.h"
#include <map>
#include <memory>
#include <cstdint>

namespace {
    struct ClientInterface {
        void* clientInterface;
        std::shared_ptr<SDK::ClientInstance> clientInstance;
    };
}

SDK::ClientInstance* SDK::MinecraftGame::getPrimaryClientInstance() {
    const auto& clients = hat::member_at<std::map<uint8_t, ClientInterface>>(this, 0x970);
    const auto primary = clients.find(0);
    return primary != clients.end() ? primary->second.clientInstance.get() : nullptr;
}
