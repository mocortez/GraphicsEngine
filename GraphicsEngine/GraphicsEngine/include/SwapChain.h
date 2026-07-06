#pragma once
#include <d3d11.h>

class SwapChain {
public:
    SwapChain();
    ~SwapChain();

    bool init(HWND hwnd, UINT width, UINT height);
    bool present();
    bool release();

    ID3D11RenderTargetView* getRenderTargetView() const { return m_rt_view; }
    ID3D11DepthStencilView* getDepthStencilView() const { return m_depth_stencil_view; }

private:
    IDXGISwapChain* m_swap_chain = nullptr;
    ID3D11RenderTargetView* m_rt_view = nullptr;
    ID3D11DepthStencilView* m_depth_stencil_view = nullptr; // CORRECCIÓN NATIVA
};
