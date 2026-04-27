#include "ConstantBuffer.h"
#include "GraphicsEngine.h"
#include <iostream>

// NOTA: No vuelvas a definir la clase aquí. 
// Usamos ConstantBuffer:: para decirle al compilador que estas funciones 
// pertenecen a la clase declarada en el .h

ConstantBuffer::ConstantBuffer() : m_buffer(nullptr)
{
}

ConstantBuffer::~ConstantBuffer()
{
    this->release();
}

bool ConstantBuffer::load(void* buffer, UINT size_buffer)
{
    if (m_buffer) m_buffer->Release();

    D3D11_BUFFER_DESC buff_desc = {};
    buff_desc.Usage = D3D11_USAGE_DEFAULT;
    buff_desc.ByteWidth = size_buffer;
    buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buff_desc.CPUAccessFlags = 0;
    buff_desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA init_data = {};
    init_data.pSysMem = buffer;

    ID3D11Device* device = GraphicsEngine::get()->getDevice();
    HRESULT hr = device->CreateBuffer(&buff_desc, &init_data, &m_buffer);

    if (FAILED(hr))
    {
        std::cout << "ConstantBuffer Error: No se pudo crear el buffer de constantes.\n";
        return false;
    }

    return true;
}

void ConstantBuffer::update(ID3D11DeviceContext* context, void* buffer)
{
    if (context && m_buffer)
    {
        context->UpdateSubresource(m_buffer, 0, NULL, buffer, 0, 0);
    }
}

bool ConstantBuffer::release()
{
    if (m_buffer)
    {
        m_buffer->Release();
        m_buffer = nullptr;
    }
    return true;
}