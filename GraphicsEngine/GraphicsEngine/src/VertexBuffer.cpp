#include "VertexBuffer.h"
#include "GraphicsEngine.h"
#include <iostream>

VertexBuffer::VertexBuffer() :
    m_buffer(nullptr),
    m_layout(nullptr),
    m_size_vertex(0),
    m_size_list(0)
{
}

VertexBuffer::~VertexBuffer()
{
    // Liberamos recursos al destruir el objeto
    this->release();
}

bool VertexBuffer::load(void* list_vertices, UINT size_vertex, UINT size_list)
{
    // 1. LIMPIEZA PREVIA
    // Si intentamos cargar datos en un buffer que ya existe, lo liberamos primero
    if (m_buffer) {
        m_buffer->Release();
        m_buffer = nullptr;
    }

    m_size_vertex = size_vertex;
    m_size_list = size_list;

    // 2. CONFIGURACIÓN DEL BUFFER
    D3D11_BUFFER_DESC buff_desc = {};
    buff_desc.Usage = D3D11_USAGE_DEFAULT;          // Memoria de GPU de lectura/escritura rápida
    buff_desc.ByteWidth = size_vertex * size_list;  // Tamaño total en bytes (debe ser múltiplo de 16/32)
    buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Indica que esto va al Input Assembler
    buff_desc.CPUAccessFlags = 0;
    buff_desc.MiscFlags = 0;

    // 3. ASIGNACIÓN DE DATOS INICIALES
    D3D11_SUBRESOURCE_DATA init_data = {};
    init_data.pSysMem = list_vertices; // Puntero a la RAM (tu lista de Point3D)

    // 4. CREACIÓN EN EL DISPOSITIVO
    // Obtenemos el dispositivo del Singleton de GraphicsEngine
    ID3D11Device* device = GraphicsEngine::get()->getDevice();
    if (!device) return false;

    HRESULT hr = device->CreateBuffer(&buff_desc, &init_data, &m_buffer);

    if (FAILED(hr))
    {
        std::cout << "VertexBuffer Error: No se pudo crear el buffer de vertices. HRESULT: " << hr << "\n";
        return false;
    }

    return true;
}

bool VertexBuffer::release()
{
    // Liberamos el buffer de la VRAM
    if (m_buffer)
    {
        m_buffer->Release();
        m_buffer = nullptr;
    }

    // El m_layout aquí es opcional si lo manejas desde GraphicsEngine, 
    // pero lo limpiamos por seguridad si se asignó.
    if (m_layout)
    {
        m_layout->Release();
        m_layout = nullptr;
    }

    return true;
}