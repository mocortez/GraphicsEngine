#include "Texture.h"
#include "GraphicsEngine.h"
#include <iostream>
#include <vector>     // CORRECCIÓN: Resuelve 'std::vector' no miembro de std
#include <wincodec.h> 

#pragma comment(lib, "Windowscodecs.lib")

Texture::Texture(const std::wstring& full_path) {
    HRESULT hr = S_OK;

    // 1. INICIALIZAR EL DECODIFICADOR NATIVO DE WINDOWS (WIC)
    IWICImagingFactory* wic_factory = nullptr;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_factory));
    if (FAILED(hr)) {
        std::cout << "Texture Error: No se pudo crear la factoria WIC.\n";
        return;
    }

    IWICBitmapDecoder* decoder = nullptr;
    hr = wic_factory->CreateDecoderFromFilename(full_path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr)) {
        std::cout << "Texture Error: No se pudo abrir o encontrar la textura JPG en la ruta especificada.\n";
        wic_factory->Release();
        return;
    }

    IWICBitmapFrameDecode* frame = nullptr;
    decoder->GetFrame(0, &frame);

    UINT width = 0, height = 0;
    frame->GetSize(&width, &height);

    // Convertir el formato del JPG nativo a RGBA de 32 bits
    IWICFormatConverter* converter = nullptr;
    wic_factory->CreateFormatConverter(&converter);
    converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);

    // 2. TRANSFERIR EXTRACCIÓN DE BYTES A LA RAM
    UINT row_pitch = width * 4;
    UINT image_size = row_pitch * height;
    std::vector<unsigned char> image_bytes(image_size);

    // CORRECCIÓN: Firma exacta de 4 argumentos para CopyPixels (Pasando NULL al RECT para copiar todo el frame)
    converter->CopyPixels(nullptr, row_pitch, image_size, (BYTE*)image_bytes.data());

    // Limpieza de COM temporales de lectura
    converter->Release();
    frame->Release();
    decoder->Release();
    wic_factory->Release();

    // 3. CREAR EL RECURSO EN LA GPU
    ID3D11Device* device = GraphicsEngine::get()->getDevice();

    D3D11_TEXTURE2D_DESC tex_desc = {};
    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags = 0;
    tex_desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA init_data = {};
    init_data.pSysMem = image_bytes.data();
    init_data.SysMemPitch = row_pitch;
    init_data.SysMemSlicePitch = image_size;

    hr = device->CreateTexture2D(&tex_desc, &init_data, &m_texture);
    if (FAILED(hr)) {
        std::cout << "Texture Error: No se pudo crear el recurso Texture2D en la GPU.\n";
        return;
    }

    // 4. CREAR LA VISTA DE RECURSO DEL SHADER (SRV)
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = tex_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(m_texture, &srv_desc, &m_shader_res_view);
    if (FAILED(hr)) {
        std::cout << "Texture Error: No se pudo crear el Shader Resource View.\n";
        return;
    }

    // 5. CREAR EL SAMPLER STATE
    D3D11_SAMPLER_DESC sampler_desc = {};
    // CORRECCIÓN: Identificador oficial de DirectX 11 (MIP con P, no MIC)
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&sampler_desc, &m_sampler_state);
    if (FAILED(hr)) {
        std::cout << "Texture Error: No se pudo crear el Sampler State.\n";
        return;
    }
}

Texture::~Texture() {
    if (m_sampler_state) { m_sampler_state->Release(); m_sampler_state = nullptr; }
    if (m_shader_res_view) { m_shader_res_view->Release(); m_shader_res_view = nullptr; }
    if (m_texture) { m_texture->Release(); m_texture = nullptr; }
}

void Texture::bind(unsigned int slot) {
    if (!m_shader_res_view || !m_sampler_state) return;

    auto ctx = GraphicsEngine::get()->getImmediateContext();
    ctx->PSSetShaderResources(slot, 1, &m_shader_res_view);
    ctx->PSSetSamplers(slot, 1, &m_sampler_state);
}