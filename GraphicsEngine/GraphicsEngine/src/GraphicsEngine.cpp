#include "GraphicsEngine.h"
#include <d3d11.h>
#include <iostream>

// Librería necesaria para vincular las funciones de DirectX
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

GraphicsEngine::GraphicsEngine() :
    m_d3d_device(nullptr), m_imm_context(nullptr), m_dxgi_device(nullptr),
    m_dxgi_adapter(nullptr), m_dxgi_factory(nullptr),
    m_vs(nullptr), m_ps(nullptr), m_layout(nullptr) {
}

GraphicsEngine::~GraphicsEngine() {}

bool GraphicsEngine::init() {
    D3D_DRIVER_TYPE driver_types[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP };
    D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };

    HRESULT hr;
    for (UINT i = 0; i < ARRAYSIZE(driver_types); i++) {
        hr = D3D11CreateDevice(NULL, driver_types[i], NULL, NULL, feature_levels,
            ARRAYSIZE(feature_levels), D3D11_SDK_VERSION,
            &m_d3d_device, NULL, &m_imm_context);
        if (SUCCEEDED(hr)) break;
    }
    if (FAILED(hr)) return false;

    // Acceso a la infraestructura DXGI (necesario para el SwapChain)
    m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
    m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
    m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);

    // Creamos los shaders inmediatamente después de iniciar el dispositivo
    return createShaders();
}

bool GraphicsEngine::createShaders() {
    ID3DBlob* vs_blob = nullptr;
    ID3DBlob* ps_blob = nullptr;
    ID3DBlob* error_blob = nullptr;

    // 1. Compilar Vertex Shader
    HRESULT hr = D3DCompileFromFile(L"Shaders/Shaders.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vs_blob, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) std::cout << (char*)error_blob->GetBufferPointer() << "\n";
        return false;
    }
    m_d3d_device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &m_vs);

    // 2. Crear el Input Layout (El mapa de tus Point3D)
    D3D11_INPUT_ELEMENT_DESC layout_desc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    m_d3d_device->CreateInputLayout(layout_desc, ARRAYSIZE(layout_desc), vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &m_layout);
    vs_blob->Release();

    // 3. Compilar Pixel Shader
    hr = D3DCompileFromFile(L"Shaders/Shaders.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &ps_blob, &error_blob);
    if (FAILED(hr)) return false;
    m_d3d_device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &m_ps);
    ps_blob->Release();

    return true;
}

bool GraphicsEngine::release() {
    if (m_layout) m_layout->Release();
    if (m_vs) m_vs->Release();
    if (m_ps) m_ps->Release();
    if (m_dxgi_factory) m_dxgi_factory->Release();
    if (m_dxgi_adapter) m_dxgi_adapter->Release();
    if (m_dxgi_device) m_dxgi_device->Release();
    if (m_imm_context) m_imm_context->Release();
    if (m_d3d_device) m_d3d_device->Release();
    return true;
}

GraphicsEngine* GraphicsEngine::get() {
    static GraphicsEngine engine;
    return &engine;
}