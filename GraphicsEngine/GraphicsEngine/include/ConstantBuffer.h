#pragma once
#include "Matrix4x4.h"
#include <d3d11.h>
#include "Prerequisites.h"

class ConstantBuffer
{
public:
    ConstantBuffer();
    ~ConstantBuffer();
    bool load(void* buffer, UINT size_buffer);
    void update(ID3D11DeviceContext* context, void* buffer);
    bool release();
    ID3D11Buffer* getBuffer() { return m_buffer; }

private:
    ID3D11Buffer* m_buffer;
};

struct CBData
{
    Matrix4x4 m_world; // Matriz de transformación del objeto
};
