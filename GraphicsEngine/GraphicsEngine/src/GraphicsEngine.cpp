#include "GraphicsEngine.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

GraphicsEngine::GraphicsEngine() :
    m_d3d_device(nullptr), m_imm_context(nullptr), m_dxgi_device(nullptr),
    m_dxgi_adapter(nullptr), m_dxgi_factory(nullptr),
    m_vs(nullptr), m_ps(nullptr), m_layout(nullptr) {
}

GraphicsEngine::~GraphicsEngine() {
    this->release();
}

bool GraphicsEngine::init() {
    D3D_DRIVER_TYPE driver_types[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP };
    D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };

    HRESULT hr;
    for (UINT i = 0; i < ARRAYSIZE(driver_types); i++) {
        hr = D3D11CreateDevice(NULL, driver_types[i], NULL, 0, feature_levels,
            ARRAYSIZE(feature_levels), D3D11_SDK_VERSION,
            &m_d3d_device, NULL, &m_imm_context);
        if (SUCCEEDED(hr)) break;
    }

    if (FAILED(hr)) return false;

    m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
    m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
    m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);

    return createShaders();
}

bool GraphicsEngine::createShaders() {
    ID3DBlob* vs_blob = nullptr;
    ID3DBlob* ps_blob = nullptr;
    ID3DBlob* error_blob = nullptr;

    HRESULT hr = D3DCompileFromFile(L"Shaders/Shaders.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vs_blob, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            std::cout << "VS Error: " << (char*)error_blob->GetBufferPointer() << "\n";
            error_blob->Release();
        }
        return false;
    }
    m_d3d_device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &m_vs);

    // CORRECCIÓN MATEMÁTICA: Offsets alineados estrictamente a la estructura contigua de 32 bytes
    D3D11_INPUT_ELEMENT_DESC layout_desc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, // Byte 12: Inmediatamente tras float3 pos
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}  // Byte 24: Inmediatamente tras float3 normal
    };

    hr = m_d3d_device->CreateInputLayout(layout_desc, ARRAYSIZE(layout_desc),
        vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &m_layout);
    vs_blob->Release();

    if (FAILED(hr)) return false;

    hr = D3DCompileFromFile(L"Shaders/Shaders.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &ps_blob, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            std::cout << "PS Error: " << (char*)error_blob->GetBufferPointer() << "\n";
            error_blob->Release();
        }
        return false;
    }
    m_d3d_device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &m_ps);
    ps_blob->Release();

    return true;
}

bool GraphicsEngine::release() {
    if (m_layout) { m_layout->Release(); m_layout = nullptr; }
    if (m_vs) { m_vs->Release(); m_vs = nullptr; }
    if (m_ps) { m_ps->Release(); m_ps = nullptr; }
    if (m_dxgi_factory) { m_dxgi_factory->Release(); m_dxgi_factory = nullptr; }
    if (m_dxgi_adapter) { m_dxgi_adapter->Release(); m_dxgi_adapter = nullptr; }
    if (m_dxgi_device) { m_dxgi_device->Release(); m_dxgi_device = nullptr; }
    if (m_imm_context) { m_imm_context->Release(); m_imm_context = nullptr; }
    if (m_d3d_device) { m_d3d_device->Release(); m_d3d_device = nullptr; }
    return true;
}

GraphicsEngine* GraphicsEngine::get() {
    static GraphicsEngine engine;
    return &engine;
}