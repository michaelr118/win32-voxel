#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

extern unsigned int D3D_XSCREEN;
extern unsigned int D3D_YSCREEN;

namespace D3D
{
	void init(HWND Wnd, unsigned int W, unsigned int H);

	void BeginRender();
	void Render(ID3D11Buffer *b);
	void EndScene();

	void SetMatrix(XMMATRIX *Matrix);
	void InitBuffer(void *d, UINT s, ID3D11Buffer **dst);

	void End();
}