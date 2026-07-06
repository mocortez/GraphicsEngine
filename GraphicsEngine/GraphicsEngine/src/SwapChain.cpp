#include "SwapChain.h"
#include "GraphicsEngine.h"
#include <iostream>

SwapChain::SwapChain() :
    m_swap_chain(nullptr), m_rt_view(nullptr), m_depth_stencil_view(nullptr) {
}

SwapChain::~SwapChain() {
    release();
}

bool SwapChain::init(HWND hwnd, UINT width, UINT height) {
    ID3D11Device* device = GraphicsEngine::get()->getDevice();
    if (!device) return false;

    // 1. CONFIGURACIÓN DEL SWAP CHAIN
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

    // 2. CREACIÓN DEL RENDER TARGET VIEW (COLOR)
    ID3D11Texture2D* buffer = nullptr;
    HRESULT hr_buf = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

    if (SUCCEEDED(hr_buf) && buffer != nullptr) {
        device->CreateRenderTargetView(buffer, NULL, &m_rt_view);
        buffer->Release();
    }
    else {
        if (m_swap_chain) { m_swap_chain->Release(); m_swap_chain = nullptr; }
        return false;
    }

    // 3. CREACIÓN DEL DEPTH BUFFER (TEXTURA DE PROFUNDIDAD FÍSICA)
    ID3D11Texture2D* depth_stencil_texture = nullptr;
    D3D11_TEXTURE2D_DESC depth_desc = {};
    depth_desc.Width = width;
    depth_desc.Height = height;
    depth_desc.MipLevels = 1;
    depth_desc.ArraySize = 1;
    depth_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits para precisión Z
    depth_desc.SampleDesc.Count = 1;
    depth_desc.SampleDesc.Quality = 0;
    depth_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_desc.CPUAccessFlags = 0;
    depth_desc.MiscFlags = 0;

    hr = device->CreateTexture2D(&depth_desc, nullptr, &depth_stencil_texture);
    if (FAILED(hr)) {
        std::cout << "SwapChain Error: No se pudo crear la textura del Depth Buffer.\n";
        return false;
    }

    // 4. CREACIÓN DE LA VISTA DE PROFUNDIDAD (DSV)
    hr = device->CreateDepthStencilView(depth_stencil_texture, nullptr, &m_depth_stencil_view);
    depth_stencil_texture->Release(); // Liberamos la referencia local de la textura limpia

    if (FAILED(hr)) {
        std::cout << "SwapChain Error: No se pudo crear el Depth Stencil View.\n";
        return false;
    }

    return true;
}

bool SwapChain::present() {
    if (m_swap_chain) {
        m_swap_chain->Present(1, 0);
    }
    return true;
}

bool SwapChain::release() {
    if (m_depth_stencil_view) {
        m_depth_stencil_view->Release();
        m_depth_stencil_view = nullptr;
    }
    if (m_rt_view) {
        m_rt_view->Release();
        m_rt_view = nullptr;
    }
    if (m_swap_chain) {
        m_swap_chain->Release();
        m_swap_chain = nullptr;
    }
    return true;
}