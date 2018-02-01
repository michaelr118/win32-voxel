#pragma once
#include "Screen.h"
#include "D3D.h"

class SDemo : public Screen
{
	XMMATRIX m_View, m_Pro;
	XMVECTOR m_Pos;
	float m_cx, m_cy;
	float m_mx, m_mz;
	ID3D11Buffer *m_Item;
	int m_ix, m_iy, m_iz;
	int m_i;

public:
	void init();

	void OnFrame();
	void Render();

	void OnKey(long long dt);
	void OnKeyLimited(unsigned int Key);
	void OnMouseButton(short Type);
	void OnMouseMove(long dx, long dy);

	void End();
};