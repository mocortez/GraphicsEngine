#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include "VertexBuffer.h"
#include "Point3D.h"
#include "Matrix4x4.h"

// Forward declarations
class SwapChain;

class AppWindow
{
public:
	AppWindow();
	~AppWindow();

	// Ciclo de vida de la aplicación
	bool init();
	bool release();
	bool isRun() const; // Marcado como const para evitar advertencias
	void broadcast();   // Sincronizado con main.cpp

	// Eventos de Renderizado
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();

protected:
	HWND m_hwnd;
	bool m_is_run;

private:
	// --- Datos y Buffer ---
	VertexBuffer* m_vb;

	// --- Componentes de DirectX 11 ---
	SwapChain* m_swap_chain;

	// --- Pipeline Matemático (MVP) ---
	Matrix4x4 m_world_matrix;
	Matrix4x4 m_view_matrix;
	Matrix4x4 m_proj_matrix;
};