#pragma once
#include <d3d11.h>
#include "Prerequisites.h"
#include "Matrix4x4.h"

// 1. Estructura de datos que viajará a la GPU
// Debe coincidir exactamente con el cbuffer en el Shaders.hlsl
struct CBData
{
    Matrix4x4 m_world; // 64 bytes (alineado a 16)
};

class ConstantBuffer
{
public:
    ConstantBuffer();
    ~ConstantBuffer();

    /**
     * @brief Crea el buffer en la VRAM de la GPU.
     * @param buffer Puntero a los datos iniciales (instancia de CBData).
     * @param size_buffer Tamaño del struct (sizeof(CBData)).
     */
    bool load(void* buffer, UINT size_buffer);

    /**
     * @brief Actualiza los datos en la GPU (ej. nueva rotación cada frame).
     * @param context El contexto inmediato del GraphicsEngine.
     * @param buffer Puntero a los nuevos datos (instancia de CBData).
     */
    void update(ID3D11DeviceContext* context, void* buffer);

    /**
     * @brief Libera el buffer de la memoria de video.
     */
    bool release();

    // Getter para vincular el buffer al pipeline en AppWindow
    ID3D11Buffer* getBuffer() { return m_buffer; }

private:
    ID3D11Buffer* m_buffer;

    // Permitimos que GraphicsEngine gestione esto si es necesario
    friend class DeviceContext;
};