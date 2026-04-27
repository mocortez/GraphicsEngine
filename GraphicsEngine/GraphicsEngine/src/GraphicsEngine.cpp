#include "GraphicsEngine.h"
#include <d3d11.h>
#include <iostream>

// Librerías necesarias para vincular las funciones de DirectX
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
    // Definimos los tipos de driver y niveles de característica (DirectX 11.0 mínimo)
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

    // --- ACCESO A LA INFRAESTRUCTURA DXGI ---
    // Necesario para que el SwapChain pueda comunicarse con el dispositivo
    m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
    m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
    m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);

    // Creamos los shaders inmediatamente
    return createShaders();
}

bool GraphicsEngine::createShaders() {
    ID3DBlob* vs_blob = nullptr;
    ID3DBlob* ps_blob = nullptr;
    ID3DBlob* error_blob = nullptr;

    // 1. Compilar Vertex Shader
    HRESULT hr = D3DCompileFromFile(L"Shaders/Shaders.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vs_blob, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            std::cout << "VS Error: " << (char*)error_blob->GetBufferPointer() << "\n";
            error_blob->Release();
        }
        return false;
    }
    m_d3d_device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &m_vs);

    // 2. Crear el Input Layout (MAPA DE MEMORIA PARA POINT3D)
    // El offset de COLOR es 16 porque: Vector3D(12) + Padding(4) = 16 bytes.
    D3D11_INPUT_ELEMENT_DESC layout_desc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    hr = m_d3d_device->CreateInputLayout(layout_desc, ARRAYSIZE(layout_desc),
        vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &m_layout);
    vs_blob->Release(); // Ya no necesitamos el código fuente compilado en RAM tras crear el layout

    if (FAILED(hr)) return false;

    // 3. Compilar Pixel Shader
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
    // Liberamos solo si el puntero no es nulo
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