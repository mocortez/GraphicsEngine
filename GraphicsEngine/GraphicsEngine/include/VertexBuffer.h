#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	bool load(void* list_vertices, UINT size_vertex, UINT size_list);
	bool release();

	// --- GETTERS PÚBLICOS ---
	UINT getSizeVertexList() const { return m_size_list; }
	UINT getVertexCount() const { return m_size_list; } // Add this public method to VertexBuffer to provide the vertex count
	ID3D11Buffer* getBuffer() { return m_buffer; } // <--- Movido aquí para que AppWindow lo vea

private:
	UINT m_size_vertex;
	UINT m_size_list;

	ID3D11Buffer* m_buffer;
	ID3D11InputLayout* m_layout;

	friend class DeviceContext;
	friend class GraphicsEngine;
};
