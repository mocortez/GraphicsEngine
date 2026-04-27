#include "AppWindow.h"
#include <iostream>
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "ConstantBuffer.h" // Nuevo: Necesario para la estructura CBData

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

AppWindow::AppWindow() : m_hwnd(nullptr), m_is_run(false), m_swap_chain(nullptr), m_vb(nullptr), m_cb(nullptr) {}
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

    // Inicializamos el motor gráfico
    if (!GraphicsEngine::get()->init()) return false;

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
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    m_swap_chain = new SwapChain();
    m_swap_chain->init(m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

    // --- 2. VERTEX DATA ---
    Point3D list[] = {
        { Vector3D(0.0f,  0.5f, 0.0f), 1.0f, 0.0f, 0.0f }, // Arriba
        { Vector3D(0.5f, -0.5f, 0.0f), 0.0f, 1.0f, 0.0f }, // Derecha
        { Vector3D(-0.5f, -0.5f, 0.0f), 0.0f, 0.0f, 1.0f }  // Izquierda
    };

    m_vb = new VertexBuffer();
    m_vb->load(list, sizeof(Point3D), ARRAYSIZE(list));

    // --- 3. CONSTANT BUFFER ---
    CBData cb;
    cb.m_world.setIdentity(); // Iniciamos con matriz identidad

    m_cb = new ConstantBuffer();
    m_cb->load(&cb, sizeof(CBData));
}

void AppWindow::onUpdate() {
    if (!m_is_run || !m_swap_chain) return;

    auto ctx = GraphicsEngine::get()->getImmediateContext();
    auto rtv = m_swap_chain->getRenderTargetView();

    // --- A. LIMPIEZA Y TARGET ---
    ctx->OMSetRenderTargets(1, &rtv, nullptr);
    float clear_color[] = { 0.1f, 0.1f, 0.15f, 1.0f };
    ctx->ClearRenderTargetView(rtv, clear_color);

    // --- B. VIEWPORT ---
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    D3D11_VIEWPORT vp = { 0, 0, (float)(rc.right - rc.left), (float)(rc.bottom - rc.top), 0.0f, 1.0f };
    ctx->RSSetViewports(1, &vp);

    // --- C. LÓGICA DE TRANSFORMACIÓN (ANIMACIÓN) ---
    CBData cb;
    cb.m_world.setIdentity();

    // Calculamos el tiempo para la rotación
    unsigned long long new_time = GetTickCount64();
    // Giramos 360 grados (2*PI) cada 5 segundos
    float delta_time = (new_time % 5000) / 5000.0f;
    float angle = delta_time * 6.2831853f;

    // Aplicamos rotación en el eje Z (para que gire en el plano de la pantalla)
    cb.m_world.setRotationZ(angle);

    // Actualizamos el buffer en la GPU
    m_cb->update(ctx, &cb);

    // --- D. BINDING PIPELINE ---
    ctx->IASetInputLayout(GraphicsEngine::get()->getInputLayout());
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(GraphicsEngine::get()->getVertexShader(), nullptr, 0);
    ctx->PSSetShader(GraphicsEngine::get()->getPixelShader(), nullptr, 0);

    // Vinculamos el Constant Buffer al Vertex Shader en el slot 0
    ID3D11Buffer* cb_raw = m_cb->getBuffer();
    ctx->VSSetConstantBuffers(0, 1, &cb_raw);

    // Vinculamos el Vertex Buffer
    UINT stride = sizeof(Point3D);
    UINT offset = 0;
    ID3D11Buffer* vb_raw = m_vb->getBuffer();
    ctx->IASetVertexBuffers(0, 1, &vb_raw, &stride, &offset);

    // --- E. DRAW & PRESENT ---
    ctx->Draw(m_vb->getSizeVertexList(), 0);
    m_swap_chain->present();
}

void AppWindow::onDestroy() {
    m_is_run = false;

    if (m_vb) {
        m_vb->release();
        delete m_vb;
        m_vb = nullptr;
    }

    if (m_cb) {
        m_cb->release();
        delete m_cb;
        m_cb = nullptr;
    }

    if (m_swap_chain) {
        m_swap_chain->release();
        delete m_swap_chain;
        m_swap_chain = nullptr;
    }

    GraphicsEngine::get()->release();
}