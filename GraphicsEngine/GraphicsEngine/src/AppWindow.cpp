#include "AppWindow.h"
#include <iostream>
#include "GraphicsEngine.h"
#include "SwapChain.h"

// Callback de la ventana (Win32)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Guardamos la referencia de nuestra clase AppWindow en la ventana de Windows
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

    // Creamos la ventana pasando 'this' como último parámetro para el WM_CREATE
    m_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MyWindowClass", L"Graphics Engine - DX11 Technical Art",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, NULL, NULL, NULL, this);

    if (!m_hwnd) return false;

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
    // Inicia el motor de gráficos
    GraphicsEngine::get()->init();

    // --- DATOS DE VÉRTICES (Puntos con Color) ---
    // Usamos el nuevo Point3D que tiene padding y Alpha para sumar 32 bytes
    Point3D list[] = {
        { Vector3D(-0.5f, -0.5f, 0.0f), 1.0f, 0.0f, 0.0f }, // Rojo
        { Vector3D(0.0f,  0.5f, 0.0f), 0.0f, 1.0f, 0.0f }, // Verde
        { Vector3D(0.5f, -0.5f, 0.0f), 0.0f, 0.0f, 1.0f }  // Azul
    };

    m_vb = new VertexBuffer();
    m_vb->load(list, sizeof(Point3D), ARRAYSIZE(list));

    // Inicializamos el SwapChain basado en el tamaño actual de la ventana
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    m_swap_chain = new SwapChain();
    m_swap_chain->init(m_hwnd, rc.right - rc.left, rc.bottom - rc.top);
}

void AppWindow::onUpdate() {
    if (!m_is_run || !m_swap_chain) return;

    // 1. LIMPIEZA DE PANTALLA
    auto ctx = GraphicsEngine::get()->getImmediateContext();
    float clear_color[] = { 0.1f, 0.1f, 0.15f, 1.0f }; // Azul oscuro profundo
    ctx->ClearRenderTargetView(m_swap_chain->getRenderTargetView(), clear_color);

    // 2. CONFIGURACIÓN DEL VIEWPORT (Dinámico)
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    D3D11_VIEWPORT vp = { 0, 0, (float)(rc.right - rc.left), (float)(rc.bottom - rc.top), 0.0f, 1.0f };
    ctx->RSSetViewports(1, &vp);

    // 3. ENLAZAR PIPELINE (Shaders y Layout)
    ctx->IASetInputLayout(GraphicsEngine::get()->getInputLayout());
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(GraphicsEngine::get()->getVertexShader(), nullptr, 0);
    ctx->PSSetShader(GraphicsEngine::get()->getPixelShader(), nullptr, 0);

    // 4. ENLAZAR DATOS (VertexBuffer)
    UINT stride = sizeof(Point3D); // Debe ser 32 bytes
    UINT offset = 0;
    ID3D11Buffer* vb_raw = m_vb->getBuffer();
    ctx->IASetVertexBuffers(0, 1, &vb_raw, &stride, &offset);

    // 5. DIBUJAR
    ctx->Draw(m_vb->getSizeVertexList(), 0);

    // 6. PRESENTAR (Con V-Sync activo dentro de SwapChain)
    m_swap_chain->present();
}

void AppWindow::onDestroy() {
    m_is_run = false;

    // Limpieza en orden inverso
    if (m_vb) {
        m_vb->release();
        delete m_vb;
        m_vb = nullptr;
    }

    if (m_swap_chain) {
        m_swap_chain->release();
        delete m_swap_chain;
        m_swap_chain = nullptr;
    }

    GraphicsEngine::get()->release();
}