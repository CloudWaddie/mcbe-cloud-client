#define NOMINMAX
#include <cstdio>
#include <Windows.h>
#include <MinHook.h>
#include <Rendering/Renderer.h>
#include <print>


static DWORD WINAPI startup(LPVOID dll) {
    MH_Initialize();
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