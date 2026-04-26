#pragma once
#include <d3d11.h>

class SwapChain
{
public:
	SwapChain();
	~SwapChain();
	bool init(HWND hwnd, UINT width, UINT height);
	bool present();
	bool release();

	ID3D11RenderTargetView* getRenderTargetView() const { return m_rt_view; }

private:
	IDXGISwapChain* m_swap_chain;
	ID3D11RenderTargetView* m_rt_view;
};
