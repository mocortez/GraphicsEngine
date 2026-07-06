#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <windows.h>
#include <d3d11.h> // CORRECCIÓN: Evita el error "missing ';' before '*'" en el tipo de interfaz de DirectX

// Incluimos las definiciones directas para evitar "use of undefined type"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "Matrix4x4.h"

class AppWindow {
public:
    AppWindow();
    ~AppWindow();

    bool init();
    bool isRun() const;
    void broadcast();

    void onCreate();
    void onUpdate();
    void onDestroy();

private:
    HWND m_hwnd;
    bool m_is_run;

    SwapChain* m_swap_chain;
    VertexBuffer* m_vb;
    ConstantBuffer* m_cb;
    Texture* m_mesh_texture;

    // CORRECCIÓN: El estado ahora sí es reconocido de forma nativa por la cabecera
    ID3D11RasterizerState* m_raster_state = nullptr;
};

#endif // APPWINDOW_H