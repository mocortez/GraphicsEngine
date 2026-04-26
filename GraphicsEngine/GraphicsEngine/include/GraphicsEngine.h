#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>

class GraphicsEngine {
public:
    GraphicsEngine();
    ~GraphicsEngine();

    // Gestión del motor
    bool init();
    bool release();
    static GraphicsEngine* get();

    // Getters para el renderizado
    ID3D11Device* getDevice() const { return m_d3d_device; }
    ID3D11DeviceContext* getImmediateContext() const { return m_imm_context; }
    ID3D11InputLayout* getInputLayout() const { return m_layout; }
    ID3D11VertexShader* getVertexShader() const { return m_vs; }
    ID3D11PixelShader* getPixelShader() const { return m_ps; }

    // Herramientas de Shaders
    bool createShaders();

private:
    // Componentes core de DirectX
    ID3D11Device* m_d3d_device;
    ID3D11DeviceContext* m_imm_context;
    IDXGIDevice* m_dxgi_device;
    IDXGIAdapter* m_dxgi_adapter;
    IDXGIFactory* m_dxgi_factory;

    // Objetos de pipeline para fotogrametría
    ID3D11VertexShader* m_vs;
    ID3D11PixelShader* m_ps;
    ID3D11InputLayout* m_layout;

    friend class SwapChain;
};