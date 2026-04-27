#include "ConstantBuffer.h"
#include "GraphicsEngine.h"

bool ConstantBuffer::load(void* buffer, UINT size_buffer) {
    D3D11_BUFFER_DESC buff_desc = {};
    buff_desc.Usage = D3D11_USAGE_DEFAULT;
    buff_desc.ByteWidth = size_buffer;
    buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buff_desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA init_data = {};
    init_data.pSysMem = buffer;

    HRESULT hr = GraphicsEngine::get()->getDevice()->CreateBuffer(&buff_desc, &init_data, &m_buffer);
    return SUCCEEDED(hr);
}

void ConstantBuffer::update(ID3D11DeviceContext* context, void* buffer) {
    context->UpdateSubresource(m_buffer, 0, NULL, buffer, 0, 0);
}

bool ConstantBuffer::release() {
    if (m_buffer) m_buffer->Release();
    return true;
}