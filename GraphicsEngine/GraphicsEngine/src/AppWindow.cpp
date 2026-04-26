#include "AppWindow.h"
#include <iostream>
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "MeshLoader.h"

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

    m_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MyWindowClass", L"Graphics Engine - Fotogrametria DJI",
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
    GraphicsEngine::get()->init();

    Point3D list[] = {
        { Vector3D(-0.5f, -0.5f, 0.0f), 1.0f, 0.0f, 0.0f },
        { Vector3D(0.0f,  0.5f, 0.0f), 0.0f, 1.0f, 0.0f },
        { Vector3D(0.5f, -0.5f, 0.0f), 0.0f, 0.0f, 1.0f }
    };

    m_vb = new VertexBuffer();
    m_vb->load(list, sizeof(Point3D), ARRAYSIZE(list));

    RECT rc;
    GetClientRect(m_hwnd, &rc);
    m_swap_chain = new SwapChain();
    m_swap_chain->init(m_hwnd, rc.right - rc.left, rc.bottom - rc.top);
}

void AppWindow::onUpdate() {
    if (!m_is_run || !m_swap_chain) return;

    auto ctx = GraphicsEngine::get()->getImmediateContext();
    float clear_color[] = { 0.0f, 0.3f, 0.4f, 1.0f };
    ctx->ClearRenderTargetView(m_swap_chain->getRenderTargetView(), clear_color);

    // Optimizamos: Usamos un tamaño fijo o guardado en lugar de llamar a GetClientRect cada frame
    D3D11_VIEWPORT vp = { 0, 0, 1280.0f, 720.0f, 0.0f, 1.0f };
    ctx->RSSetViewports(1, &vp);

    ctx->IASetInputLayout(GraphicsEngine::get()->getInputLayout());
    ctx->VSSetShader(GraphicsEngine::get()->getVertexShader(), nullptr, 0);
    ctx->PSSetShader(GraphicsEngine::get()->getPixelShader(), nullptr, 0);

    UINT stride = sizeof(Point3D);
    UINT offset = 0;
    ID3D11Buffer* vb_raw = m_vb->getBuffer();
    ctx->IASetVertexBuffers(0, 1, &vb_raw, &stride, &offset);

    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->Draw(m_vb->getSizeVertexList(), 0);

    m_swap_chain->present();
}

void AppWindow::onDestroy() {
    m_is_run = false; // Detiene el bucle en main.cpp
    if (m_vb) { m_vb->release(); delete m_vb; m_vb = nullptr; }
    if (m_swap_chain) { m_swap_chain->release(); delete m_swap_chain; m_swap_chain = nullptr; }
    GraphicsEngine::get()->release();
}