//
// Created by Edward on 24/10/2026.
//

#include "CIUpdate.h"

#include <MinHook.h>

#include <ImGui.h>
#include "libhat/scanner.hpp"
#include "libhat/signature.hpp"
#include "SDK/game/ClientInstance.h"
#include "SDK/memory/sigmanager/SigManager.h"
#include "Utils/log.h"

using ciUpdateFunc = bool(*)(SDK::ClientInstance*, bool);
static ciUpdateFunc oCiUpdate{};
static std::byte* ciUpdateAddress{};

static bool hk_ciupdate(SDK::ClientInstance* self, bool isInitFinished) {
   SDK::ClientInstance::setInstance(self);
   LOG("found ci update at: %p\n", static_cast<void*>(ciUpdateAddress));
   MH_DisableHook(ciUpdateAddress);
   return oCiUpdate(self, isInitFinished);
}

void Hooks::CIUpdate::init() {
   auto sig = SDK::SigManager::ClientInstance_Update;
   hat::scan_result scanResult = hat::find_pattern(sig, ".text");
   ciUpdateAddress = scanResult.get();
   if (!ciUpdateAddress) {
      LOG("failed to find ci update");
      return;
   }

   MH_CreateHook(ciUpdateAddress, reinterpret_cast<LPVOID>(&hk_ciupdate), reinterpret_cast<LPVOID*>(&oCiUpdate));
   MH_EnableHook(ciUpdateAddress);
}
