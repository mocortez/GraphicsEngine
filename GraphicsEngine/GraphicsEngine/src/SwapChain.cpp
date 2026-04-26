#include "SwapChain.h"
#include "GraphicsEngine.h"

SwapChain::SwapChain() : m_swap_chain(nullptr), m_rt_view(nullptr) {}
SwapChain::~SwapChain() { release(); }

bool SwapChain::init(HWND hwnd, UINT width, UINT height) {
    ID3D11Device* device = GraphicsEngine::get()->getDevice();

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 1;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = hwnd;
    desc.SampleDesc.Count = 1;
    desc.Windowed = TRUE;

    HRESULT hr = GraphicsEngine::get()->m_dxgi_factory->CreateSwapChain(device, &desc, &m_swap_chain);
    if (FAILED(hr)) return false;

    ID3D11Texture2D* buffer = nullptr;
    m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
    device->CreateRenderTargetView(buffer, NULL, &m_rt_view);
    buffer->Release();

    return true;
}

bool SwapChain::present() {
    // CAMBIO CRÍTICO: El primer parámetro en '1' activa V-Sync. 
    // Esto evita que tu PC se congele al limitar la velocidad a 60 FPS.
    m_swap_chain->Present(1, 0);
    return true;
}

bool SwapChain::release() {
    if (m_rt_view) m_rt_view->Release();
    if (m_swap_chain) m_swap_chain->Release();
    return true;
}