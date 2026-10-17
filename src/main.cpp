#define NOMINMAX
#include <Windows.h>
#include <cstdlib>
#include <MinHook.h>
#include <Rendering/Renderer.h>

#include "SDK/game/ClientInstance.h"

static DWORD WINAPI startup(LPVOID dll) {
    auto status = MH_Initialize();

    if (status != MH_OK &&
        status != MH_ERROR_ALREADY_INITIALIZED)
        return 1;

    if (!Rendering::init()) {
        OutputDebugStringA("Renderer initialization failed\n");
        return 1;
    }

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