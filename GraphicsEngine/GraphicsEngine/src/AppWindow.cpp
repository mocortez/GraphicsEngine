#include "AppWindow.h"
#include <iostream>
#include <vector>

#include "Vector3D.h"
#include "Point3D.h"    
#include "MeshLoader.h" 
#include "GraphicsEngine.h"

struct AppWindowCBData {
    Matrix4x4 m_world;
    Matrix4x4 m_view;
    Matrix4x4 m_proj;
    float m_has_texture;
    float padding[3];
};

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

AppWindow::AppWindow() :
    m_hwnd(nullptr), m_is_run(false), m_swap_chain(nullptr), m_vb(nullptr), m_cb(nullptr),
    m_mesh_texture(nullptr), m_raster_state(nullptr) {
}

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

    m_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MyWindowClass", L"Graphics Engine - Monument Render",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, NULL, NULL, NULL, this);

    if (!m_hwnd) return false;

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
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    m_swap_chain = new SwapChain();
    m_swap_chain->init(m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

    std::vector<Point3D> model_vertices;
    std::string model_path = R"(E:\Av Independencia lowpoly\1\Av Independencia Lowpoly.obj)";

    std::cout << "Cargando malla geometrica: " << model_path << "\n";

    if (!MeshLoader::LoadOBJ(model_path, model_vertices)) {
        std::cout << "Error critico al cargar OBJ. Usando triangulo de respaldo...\n";
        model_vertices.clear();
        model_vertices.push_back(Point3D(Vector3D(0.0f, 0.5f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f), MeshUV(0.0f, 0.0f)));
        model_vertices.push_back(Point3D(Vector3D(0.5f, -0.5f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f), MeshUV(1.0f, 1.0f)));
        model_vertices.push_back(Point3D(Vector3D(-0.5f, -0.5f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f), MeshUV(0.0f, 1.0f)));
    }

    m_vb = new VertexBuffer();
    m_vb->load(model_vertices.data(), sizeof(Point3D), (UINT)model_vertices.size());

    m_mesh_texture = new Texture(LR"(E:\Av Independencia lowpoly\Special\textura.jpg)");

    D3D11_RASTERIZER_DESC raster_desc = {};
    raster_desc.FillMode = D3D11_FILL_SOLID;
    raster_desc.CullMode = D3D11_CULL_NONE;
    raster_desc.DepthClipEnable = TRUE;

    GraphicsEngine::get()->getDevice()->CreateRasterizerState(&raster_desc, &m_raster_state);

    AppWindowCBData cb;
    cb.m_world.setIdentity();
    cb.m_view.setIdentity();
    cb.m_proj.setIdentity();
    cb.m_has_texture = 0.0f;

    m_cb = new ConstantBuffer();
    m_cb->load(&cb, sizeof(AppWindowCBData));
}

void AppWindow::onUpdate() {
    if (!m_is_run || !m_swap_chain) return;

    auto ctx = GraphicsEngine::get()->getImmediateContext();
    auto rtv = m_swap_chain->getRenderTargetView();
    // 1. CAPTURA DEL DEPTH VIEW DESDE EL SWAP CHAIN
    auto dsv = m_swap_chain->getDepthStencilView();

    // CORRECCIÓN: Vinculamos el DSV en lugar de pasar nullptr
    ctx->OMSetRenderTargets(1, &rtv, dsv);

    float clear_color[] = { 0.12f, 0.12f, 0.18f, 1.0f };
    ctx->ClearRenderTargetView(rtv, clear_color);

    // CORRECCIÓN: Limpieza física del Z-Buffer a 1.0f en cada frame para evitar artefactos
    if (dsv) {
        ctx->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    RECT rc;
    GetClientRect(m_hwnd, &rc);
    float width = (float)(rc.right - rc.left);
    float height = (float)(rc.bottom - rc.top);

    D3D11_VIEWPORT vp = { 0, 0, width, height, 0.0f, 1.0f };
    ctx->RSSetViewports(1, &vp);

    float aspect = (height > 0) ? (width / height) : 1.0f;

    AppWindowCBData cb;

    unsigned long long new_time = GetTickCount64();
    float delta_time = (new_time % 16000) / 16000.0f;
    float angle = delta_time * 6.2831853f;

    Matrix4x4 mat_scale, mat_rot, mat_translation;

    mat_scale.setScale(-1.0f, 1.0f, 1.0f);
    mat_rot.setRotationY(angle);
    mat_translation.setTranslation(0.0f, 0.0f, 0.0f);

    // Escala -> Rotación -> Traslación (Coherente con Column-Major en Matrix4x4.h)
    cb.m_world = mat_scale * mat_rot * mat_translation;

    // Matriz de Vista: Posicionamos la cámara a una distancia segura en el eje Z positivo
    cb.m_view.setIdentity();
    cb.m_view.setTranslation(0.0f, 1.4f, 6.0f);

    // Matriz de Proyección
    cb.m_proj.setIdentity();
    cb.m_proj.setPerspectiveFovLH(1.57f, aspect, 0.1f, 100.0f);

    if (m_mesh_texture && m_mesh_texture->isValid()) {
        cb.m_has_texture = 1.0f;
    }
    else {
        cb.m_has_texture = 0.0f;
    }

    m_cb->update(ctx, &cb);

    if (m_raster_state) {
        ctx->RSSetState(m_raster_state);
    }

    ctx->IASetInputLayout(GraphicsEngine::get()->getInputLayout());
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(GraphicsEngine::get()->getVertexShader(), nullptr, 0);
    ctx->PSSetShader(GraphicsEngine::get()->getPixelShader(), nullptr, 0);

    ID3D11Buffer* d3d_cb = m_cb->getBuffer();
    ctx->VSSetConstantBuffers(0, 1, &d3d_cb);
    ctx->PSSetConstantBuffers(0, 1, &d3d_cb);

    UINT stride = sizeof(Point3D);
    UINT offset = 0;
    ID3D11Buffer* vb = m_vb->getBuffer();
    ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);

    if (cb.m_has_texture > 0.5f) {
        m_mesh_texture->bind(0);
    }

    ctx->Draw(m_vb->getSizeVertexList(), 0);

    m_swap_chain->present();
}

void AppWindow::onDestroy() {
    m_is_run = false;
    if (m_raster_state) m_raster_state->Release();
    if (m_mesh_texture) delete m_mesh_texture;
    if (m_vb) { m_vb->release(); delete m_vb; }
    if (m_cb) { m_cb->release(); delete m_cb; }
    if (m_swap_chain) { m_swap_chain->release(); delete m_swap_chain; }
    GraphicsEngine::get()->release();
}