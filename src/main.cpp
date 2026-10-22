#include <cstdio>
#include <Windows.h>
#include <MinHook.h>
#include <Rendering/Renderer.h>
#include <print>
#include "Features/FeatureManager.h"
#include "Utils/log.h"
#include "Globals.h"

static DWORD WINAPI startup(LPVOID dll) {
    #ifndef NDEBUG
        AllocConsole();
        FILE* fout;
        FILE* ferr;
        freopen_s(&fout, "CONOUT$", "w", stdout);
        freopen_s(&ferr, "CONOUT$", "w", stderr);
    #endif

    LOG("Initializing minhook\n");
    MH_Initialize();
    LOG("FeatureManager::init\n"); // init before render so when i add the event system for RenderEvent the events are already ready
    FeatureManager::init();
    LOG("Rendering::init\n");
    Rendering::init();
    while (!Client::shouldUninject) {
        Sleep(10);
    }
    MH_DisableHook(MH_ALL_HOOKS);
    Sleep(10); // short sleep
    Rendering::shutdown();
    FeatureManager::shutdown();
    MH_Uninitialize();
    #ifndef NDEBUG
        fclose(fout);
        fclose(ferr);
        FreeConsole();
    #endif
    FreeLibraryAndExitThread(static_cast<HMODULE>(dll), 0);
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