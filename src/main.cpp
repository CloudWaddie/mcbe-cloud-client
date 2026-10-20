#define NOMINMAX
#include <cstdio>
#include <Windows.h>
#include <MinHook.h>
#include <Rendering/Renderer.h>
#include <print>
#include "Features/FeatureManager.h"
#include "Utils/log.h"

static DWORD WINAPI startup(LPVOID dll) {
    #ifndef NDEBUG
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
    #endif

    LOG("Initializing minhook\n");
    MH_Initialize();
    LOG("Rendering::init\n");
    Rendering::init();
    LOG("FeatureManager::init\n");
    FeatureManager::init();
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        // injected
        CreateThread(nullptr, 0, &startup, hinst, 0, nullptr);
        break;
    case DLL_THREAD_ATTACH:
        // dont need anything
    default:
        break;
    }

    return TRUE;
}