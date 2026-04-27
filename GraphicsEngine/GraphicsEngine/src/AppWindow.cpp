#include "AppWindow.h"
#include <iostream>
#include "GraphicsEngine.h"
#include "SwapChain.h"

// Callback de la ventana (Win32)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        AppWindow* window = (AppWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        break;
    }
    case WM_DESTROY: {
        AppWindow* window = (AppWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (window) window->onDestroy();
        PostQuitMessage(0);
        break;
    }
    default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return NULL;
}

AppWindow::AppWindow() : m_hwnd(nullptr), m_is_run(false), m_swap_chain(nullptr), m_vb(nullptr) {}
AppWindow::~AppWindow() {}

bool AppWindow::init() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"MyWindowClass";
    wc.lpfnWndProc = &WindowProc;

    if (!RegisterClassEx(&wc)) return false;

    m_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MyWindowClass", L"Graphics Engine - DX11 Technical Art",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, NULL, NULL, NULL, this);

    if (!m_hwnd) return false;

    // 1. Iniciamos el motor de gráficos PRIMERO
    if (!GraphicsEngine::get()->init()) {
        return false;
    }

    // 2. Ejecutamos nuestra lógica de creación de recursos
    this->onCreate();

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    m_is_run = true;
    return true;
}

void AppWindow::broadcast() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    this->onUpdate();
}

bool AppWindow::isRun() const { return m_is_run; }

void AppWindow::onCreate() {
    // --- 1. SWAPCHAIN ---
    // Lo creamos primero para tener dónde dibujar
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    m_swap_chain = new SwapChain();
    m_swap_chain->init(m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

    // --- 2. DATOS DE VÉRTICES (Sentido Horario / Clockwise) ---
    // DirectX 11 descarta por defecto lo que no esté en este orden
    Point3D list[] = {
        { Vector3D(0.0f,  0.5f, 0.0f), 1.0f, 0.0f, 0.0f }, // Arriba (Rojo)
        { Vector3D(0.5f, -0.5f, 0.0f), 0.0f, 1.0f, 0.0f }, // Derecha abajo (Verde)
        { Vector3D(-0.5f, -0.5f, 0.0f), 0.0f, 0.0f, 1.0f }  // Izquierda abajo (Azul)
    };

    // --- 3. VERTEX BUFFER ---
    m_vb = new VertexBuffer();
    m_vb->load(list, sizeof(Point3D), ARRAYSIZE(list));
}

void AppWindow::onUpdate() {
    if (!m_is_run || !m_swap_chain) return;

    auto ctx = GraphicsEngine::get()->getImmediateContext();
    auto rtv = m_swap_chain->getRenderTargetView();

    // --- A. VINCULAR EL RENDER TARGET ---
    // ¡ESTO ES LO QUE FALTABA! Le dice a la GPU dónde escribir los pixeles
    ctx->OMSetRenderTargets(1, &rtv, nullptr);

    // --- B. LIMPIEZA DE PANTALLA ---
    float clear_color[] = { 0.1f, 0.1f, 0.15f, 1.0f };
    ctx->ClearRenderTargetView(rtv, clear_color);

    // --- C. CONFIGURACIÓN DEL VIEWPORT ---
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    D3D11_VIEWPORT vp = { 0, 0, (float)(rc.right - rc.left), (float)(rc.bottom - rc.top), 0.0f, 1.0f };
    ctx->RSSetViewports(1, &vp);

    // --- D. ENLAZAR PIPELINE ---
    ctx->IASetInputLayout(GraphicsEngine::get()->getInputLayout());
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(GraphicsEngine::get()->getVertexShader(), nullptr, 0);
    ctx->PSSetShader(GraphicsEngine::get()->getPixelShader(), nullptr, 0);

    // --- E. ENLAZAR DATOS (VertexBuffer) ---
    UINT stride = sizeof(Point3D);
    UINT offset = 0;
    ID3D11Buffer* vb_raw = m_vb->getBuffer();
    ctx->IASetVertexBuffers(0, 1, &vb_raw, &stride, &offset);

    // --- F. DIBUJAR ---
    ctx->Draw(m_vb->getSizeVertexList(), 0);

    // --- G. PRESENTAR ---
    m_swap_chain->present();
}

void AppWindow::onDestroy() {
    m_is_run = false;

    if (m_vb) {
        m_vb->release(); // Libera el buffer interno de DX11
        delete m_vb;     // Borra el objeto C++
        m_vb = nullptr;
    }

    if (m_swap_chain) {
        m_swap_chain->release(); // Libera la cadena de intercambio
        delete m_swap_chain;
        m_swap_chain = nullptr;
    }

    // Cerramos el motor al final de todo
    GraphicsEngine::get()->release();
}