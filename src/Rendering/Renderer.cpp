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



// all vars we need for rendering
static winrt::com_ptr<ID3D11Device> d3d11Device;
static winrt::com_ptr<ID3D11DeviceContext> d3d11Context;
static winrt::com_ptr<ID3D12Device> d3d12Device; // we only need this to check if we have the directx12 enabled
static winrt::com_ptr<ID3D11On12Device> d3d11on12Device;
static winrt::com_ptr<IDXGISwapChain3> swapChain3; // we can get back buffers from this
static winrt::com_ptr<ID3D12CommandQueue> commandQueue; // to get command queue for d3d11on12

using executeCommandQueueFunc = void(*)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);
static executeCommandQueueFunc oExecuteCommandLists{};

static void hk_ExecuteCommandLists(ID3D12CommandQueue* queue, UINT numCommandLists, ID3D12CommandList* const* commandLists) {
    if (!commandQueue && queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
        commandQueue.copy_from(queue);
    }
    oExecuteCommandLists(queue, numCommandLists, commandLists);
}


static bool imguiBackendReady = false;
static bool framesReady = false;


using presentFunc = HRESULT(*)(IDXGISwapChain*, UINT, UINT);
static presentFunc oPresent{};

static HRESULT hk_Present(IDXGISwapChain* this_, UINT SyncInterval, UINT Flags) {
    if (!imguiBackendReady) {
        DXGI_SWAP_CHAIN_DESC desc{};
        this_->GetDesc(&desc);
        HWND hwnd = desc.OutputWindow;
        ImGui_ImplWin32_Init(hwnd);

        if (SUCCEEDED(this_->GetDevice(IID_PPV_ARGS(d3d12Device.put())))) {
            // dd3d11on12
            IUnknown* queues[] = { commandQueue.get() };
            UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
            HRESULT hr = D3D11On12CreateDevice(d3d12Device.get(), D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, queues, 1, 0, d3d11Device.put(), d3d11Context.put(), nullptr);
            if (SUCCEEDED(hr)) {
                d3d11Device.as(d3d11on12Device);
            }
        }
        else if (SUCCEEDED(this_->GetDevice(IID_PPV_ARGS(d3d11Device.put())))) {
            // directx11
            d3d11Device->GetImmediateContext(d3d11Context.put());
            ImGui_ImplDX11_Init(d3d11Device.get(), d3d11Context.get());
            imguiBackendReady = true;
        }
    }
    if (imguiBackendReady) {
        // now render imgui frames
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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
}