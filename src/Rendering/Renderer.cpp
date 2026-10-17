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

using executeCommandQueueFunc = void(*)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);
using presentFunc = HRESULT(*)(IDXGISwapChain*, UINT, UINT);

static executeCommandQueueFunc oExecuteCommandLists{};
static presentFunc oPresent{};

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
        ImGui::ShowDemoWindow();
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
    return true;
}
