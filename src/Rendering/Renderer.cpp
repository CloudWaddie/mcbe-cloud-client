#include <kiero.hpp>
#include "Renderer.h"

// render libs
#include <dxgi1_4.h>
#include <d3d11.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <Windows.h>
#include <winrt/base.h>

// imgui but since we are doing d3d11on12 we dont need dx12 headers :)
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <vector>
#include <utility>

// minhook for window messages
#include <MinHook.h>

// game input
#include "Notifications.h"
#include "../GameInput/GameInput.h"
#include "Utils/log.h"

// all vars we need for rendering
static winrt::com_ptr<ID3D11Device> d3d11Device;
static winrt::com_ptr<ID3D11DeviceContext> d3d11Context;
static winrt::com_ptr<ID3D12Device> d3d12Device; // we only need this to check if we have the directx12 enabled
static winrt::com_ptr<ID3D11On12Device> d3d11on12Device;
static winrt::com_ptr<IDXGISwapChain3> swapChain3; // we can get back buffers from this
static winrt::com_ptr<ID3D12CommandQueue> commandQueue; // to get command queue for d3d11on12
static std::vector<winrt::com_ptr<ID3D11Resource>> wrappedBackBuffers; // buffer
static std::vector<winrt::com_ptr<ID3D11RenderTargetView>> renderTargetViews; // points to the back buffer
static bool imguiBackendReady = false;
static bool framesReady = false;
static bool win32Ready = false;
static bool wndProcHooked = false;
static bool welcomed = false;

using executeCommandQueueFunc = void(*)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);
using presentFunc = HRESULT(*)(IDXGISwapChain*, UINT, UINT);
using gameMouseStateFunc = bool(*)(GameInput::v2::IGameInputReading*, GameInput::v2::GameInputMouseState*);

using resizeBuffers1Func = HRESULT(*)(IDXGISwapChain3*, UINT, UINT, UINT, DXGI_FORMAT, UINT, const UINT*, IUnknown* const*);
static resizeBuffers1Func oResizeBuffers1{};

static executeCommandQueueFunc oExecuteCommandLists{};
static presentFunc oPresent{};
static WNDPROC oWndProc = nullptr;
static gameMouseStateFunc oGameMouseState{};
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void cleanupFrames() {
    framesReady = false;
    if (d3d11Context) {
        if (d3d11on12Device) {
            wrappedBackBuffers.resize(0);
            renderTargetViews.resize(0);
        }
        d3d11Context->Flush();
    }
}


static HRESULT hk_ResizeBuffers1(
    IDXGISwapChain3* self,
    UINT bufferCount,
    UINT width,
    UINT height,
    DXGI_FORMAT format,
    UINT flags,
    const UINT* creationNodeMask,
    IUnknown* const* presentQueue
) {
    cleanupFrames();

    return oResizeBuffers1(
        self,
        bufferCount,
        width,
        height,
        format,
        flags,
        creationNodeMask,
        presentQueue
    );
}


static bool hk_GameMouseState(
    GameInput::v2::IGameInputReading* self,
    GameInput::v2::GameInputMouseState* mouseState
) {
    bool result = oGameMouseState(self, mouseState);

    if (result && mouseState && ImGui::GetCurrentContext()) {
        ImGuiIO& io = ImGui::GetIO();

        if (io.WantCaptureMouse) {
            // Suppress mouse input reaching the game.
            *mouseState = {};
        }
    }

    return result;
}

LRESULT CALLBACK hk_windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui::GetCurrentContext()) {
        LRESULT result = ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
        ImGuiIO& io = ImGui::GetIO();
        // imgui already handles these from the handler and the game reads through GameInput rather than windowProc so this does nothing
        /*if (io.WantCaptureMouse) {
            switch (msg) {
                case WM_MOUSEMOVE:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                case WM_MOUSEWHEEL:
                case WM_MOUSEHWHEEL:
                    return 0;
                case WM_XBUTTONDOWN:
                case WM_XBUTTONUP:
                case WM_XBUTTONDBLCLK:
                    return TRUE;
            }
        }*/
        if (io.WantCaptureKeyboard) {
            switch (msg) {
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_CHAR:
                case WM_SYSCHAR:
                    return 0;
            }
        }

        if (result) {
            return result;
        }
    }
    return CallWindowProc(oWndProc, hwnd, msg, wParam, lParam);
}


static HWND hookedWindow = nullptr;

static bool hookWndProc(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return false;

    SetLastError(0);

    auto previous = SetWindowLongPtr(
        hwnd,
        GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(&hk_windowProc)
    );

    if (!previous) {
        return false;
    }

    oWndProc = reinterpret_cast<WNDPROC>(previous);
    hookedWindow = hwnd;

    return true;
}

static bool createRenderTargets(IDXGISwapChain* swapChain) {
    if (!d3d11Device || !d3d11on12Device) {
        return false;
    }
    // we need swapchain3 because it has the method to get the backbuffer but because we are
    // using the com objects we can actually get that method
    winrt::com_ptr<IDXGISwapChain3> sc3;
    if (FAILED(swapChain->QueryInterface(IID_PPV_ARGS(sc3.put())))) {
        return false;
    }
    // now sc3 has the methods for both
    DXGI_SWAP_CHAIN_DESC desc{};

    if (FAILED(swapChain->GetDesc(&desc)) || desc.BufferCount == 0) {
        return false; // if we cant get the description or there are no buffers there's no point trying to get the backbuffer so stop
    }
    std::vector<winrt::com_ptr<ID3D11Resource>> newBuffers(desc.BufferCount);
    std::vector<winrt::com_ptr<ID3D11RenderTargetView>> newRTVs(desc.BufferCount);

    D3D11_RESOURCE_FLAGS flags{};
    flags.BindFlags = D3D11_BIND_RENDER_TARGET;

    // now we create an rtv for eaach back buffer
    for (UINT i = 0; i < desc.BufferCount; i++) {
        winrt::com_ptr<ID3D12Resource> backBuffer;

        HRESULT hr = swapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer.put()));

        if (FAILED(hr)) {
            return false;
        }

        // now we wrap it because it is a dx12 thing and we want to use with dx11
        hr = d3d11on12Device->CreateWrappedResource(backBuffer.get(), &flags, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS((newBuffers[i].put())));
        if (FAILED(hr)) {
            return false;
        }

        hr = d3d11Device->CreateRenderTargetView(newBuffers[i].get(), nullptr, newRTVs[i].put());

        if (FAILED(hr)) {
            return false;
        }
    }
    swapChain3 = std::move(sc3);
    wrappedBackBuffers = std::move(newBuffers);
    renderTargetViews = std::move(newRTVs);
    return true;
}

static void hk_ExecuteCommandLists(ID3D12CommandQueue* queue, UINT numCommandLists, ID3D12CommandList* const* commandLists) {
    if (!commandQueue && queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
        commandQueue.copy_from(queue);
    }
    oExecuteCommandLists(queue, numCommandLists, commandLists);
}


static HRESULT hk_Present(IDXGISwapChain* this_, UINT SyncInterval, UINT Flags) {
    if (!imguiBackendReady) {
        DXGI_SWAP_CHAIN_DESC desc{};
        this_->GetDesc(&desc);
        HWND hwnd = desc.OutputWindow;
        if (hwnd && !win32Ready) {
            win32Ready = ImGui_ImplWin32_Init(hwnd);
        }
        if (hwnd && !wndProcHooked && win32Ready ) {
            wndProcHooked = hookWndProc(hwnd);
            winrt::com_ptr<GameInput::v2::IGameInput> gameInput{};
            winrt::check_hresult(GameInput::v2::GameInputCreate(gameInput.put()));

            winrt::com_ptr<GameInput::v2::IGameInputReading> gameInputReading{};
            winrt::check_hresult(gameInput->GetCurrentReading(GameInput::v2::GameInputKind::GameInputKindMouse, nullptr,
                                                              gameInputReading.put()));

            constexpr size_t vtableIndex = 14;

            auto vtable = *reinterpret_cast<void***>(gameInputReading.get());
            void* function = vtable[vtableIndex];

            auto status = MH_CreateHook(
            function,
                reinterpret_cast<LPVOID>(&hk_GameMouseState),
                reinterpret_cast<LPVOID*>(&oGameMouseState)
            );
            if (status == MH_OK) {
                status = MH_EnableHook(
                    reinterpret_cast<LPVOID>(function)
                );
            }
        }
        if (!d3d12Device) {
            this_->GetDevice(IID_PPV_ARGS(d3d12Device.put()));
        }
        if (d3d12Device) {
            // dd3d11on12
            if (!commandQueue) {
                return oPresent(this_, SyncInterval, Flags);
            }
            IUnknown* queues[] = { commandQueue.get() };
            if (!d3d11on12Device) {
                HRESULT hr = D3D11On12CreateDevice(d3d12Device.get(), D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, queues, 1, 0, d3d11Device.put(), d3d11Context.put(), nullptr);
                if (SUCCEEDED(hr)) {
                    d3d11Device.as(d3d11on12Device);
                }
            }
        }
        else if (SUCCEEDED(this_->GetDevice(IID_PPV_ARGS(d3d11Device.put())))) {
            // directx11
            d3d11Device->GetImmediateContext(d3d11Context.put());
        }
    }
    if (d3d11on12Device && !framesReady) {
        framesReady = createRenderTargets(this_);
    }
    if (!imguiBackendReady && win32Ready && d3d11Device && d3d11Context) {
        imguiBackendReady = ImGui_ImplDX11_Init(
            d3d11Device.get(),
            d3d11Context.get()
        );
    }
    if (imguiBackendReady && framesReady && d3d11on12Device && swapChain3) {

        // set the rtv
        UINT index = swapChain3->GetCurrentBackBufferIndex();
        if (index >= wrappedBackBuffers.size() || index >= renderTargetViews.size()) {
            return oPresent(this_, SyncInterval, Flags);
        }

        ID3D11Resource* wrapped = wrappedBackBuffers[index].get();
        ID3D11RenderTargetView* rtv = renderTargetViews[index].get();

        // now render imgui frames
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        if (!welcomed) {
            Rendering::Notifications::notify("Successfully injected!");
            welcomed = true;
        };

        Rendering::Notifications::render();

        ImGui::Render();

        d3d11on12Device->AcquireWrappedResources(&wrapped, 1);
        d3d11Context->OMSetRenderTargets(1, &rtv, nullptr);
        // render using our rtv
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        // cleanup
        d3d11Context->OMSetRenderTargets(0, nullptr, nullptr);
        d3d11on12Device->ReleaseWrappedResources(&wrapped, 1);
        d3d11Context->Flush();
    }

    return oPresent(this_, SyncInterval, Flags);
}

bool Rendering::init() {
    if (kiero::init(kiero::RenderType::Auto) != kiero::Status::Success) {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    kiero::bind<&ID3D12CommandQueue::ExecuteCommandLists>(&oExecuteCommandLists, &hk_ExecuteCommandLists);
    auto status = kiero::bind<&IDXGISwapChain::Present>(&oPresent, &hk_Present);
    assert(status == kiero::Status::Success);
    auto resizeStatus = kiero::bind<&IDXGISwapChain3::ResizeBuffers1>(&oResizeBuffers1,&hk_ResizeBuffers1);
    // assert(resizeStatus == kiero::Status::Success); // honestly if we're just gonna not render i might as well just let it continue and if it crashes so be it
    return true;
}
