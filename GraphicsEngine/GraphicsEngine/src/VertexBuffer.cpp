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
    // Delegamos la limpieza al método release
    this->release();
}

bool VertexBuffer::load(void* list_vertices, UINT size_vertex, UINT size_list)
{
    // Si ya existía un buffer (por ejemplo, al recargar un nuevo OBJ), lo liberamos
    if (m_buffer) m_buffer->Release();
    if (m_layout) m_layout->Release();

    m_size_vertex = size_vertex;
    m_size_list = size_list;

    // 1. CONFIGURACIÓN DEL BUFFER (La "Caja" en la VRAM)
    D3D11_BUFFER_DESC buff_desc = {};
    buff_desc.Usage = D3D11_USAGE_DEFAULT;          // Optimizado para que la GPU lo lea rápido
    buff_desc.ByteWidth = size_vertex * size_list;  // Tamaño total: (Puntos * Bytes por punto)
    buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Indica que contiene vértices
    buff_desc.CPUAccessFlags = 0;                   // El CPU no necesita leerlo de vuelta
    buff_desc.MiscFlags = 0;

    // 2. MAPEO DE DATOS (El contenido)
    D3D11_SUBRESOURCE_DATA init_data = {};
    init_data.pSysMem = list_vertices; // Puntero a tus datos Point3D en la RAM

    // 3. CREACIÓN FÍSICA EN LA GPU
    // Accedemos al Device que configuramos en el GraphicsEngine
    HRESULT hr = GraphicsEngine::get()->getDevice()->CreateBuffer(&buff_desc, &init_data, &m_buffer);

    if (FAILED(hr))
    {
        std::cout << "Error Crítico: No se pudo crear el VertexBuffer en la GPU.\n";
        return false;
    }

    return true;
}

bool VertexBuffer::release()
{
    // Liberar en orden inverso a la creación es una buena práctica
    if (m_layout)
    {
        m_layout->Release();
        m_layout = nullptr;
    }

    if (m_buffer)
    {
        m_buffer->Release();
        m_buffer = nullptr;
    }

    // No usamos 'delete this' aquí porque queremos que AppWindow 
    // controle la vida del objeto VertexBuffer*
    return true;
}