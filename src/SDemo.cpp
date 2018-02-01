#include "SDemo.h"
#include "Map.h"
#include "Model.h"

#define MATH_PI 3.14159265359f
#define MATH_RADIAN(d) (d * (MATH_PI / 180.0f))

void SDemo::init()
{
	m_Pos = XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f);
	m_Pro = XMMatrixPerspectiveFovLH(MATH_RADIAN(45.0f), ((float)D3D_XSCREEN
		/ (float)D3D_YSCREEN), 0.1f, 100.0f);

	m_Item = nullptr;
	m_i = 0;

	m_cx = m_cy = m_mx = m_mz = 0.0f;
	m_ix = m_iy = m_iz = 0;

	Map::init(XMVectorGetX(m_Pos), XMVectorGetZ(m_Pos));
}

void SDemo::OnFrame()
{
	XMMATRIX m = XMMatrixRotationRollPitchYaw(m_cy, m_cx, 0.0f);

	XMVECTOR Fwd = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR Ri = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	m_Pos += (XMVector3TransformCoord(Fwd, m) * m_mz);
	m_Pos += (XMVector3TransformCoord(Ri, m) * m_mx);
	m_Pos = XMVectorSetY(m_Pos, 2.0f);

	m_mx = m_mz = 0.0f;

	if (XMVectorGetX(m_Pos) < 0.0f) {
		m_Pos = XMVectorSetX(m_Pos, 0.0f);
	}

	if (XMVectorGetZ(m_Pos) < 0.0f) {
		m_Pos = XMVectorSetZ(m_Pos, 0.0f);
	}

	XMVECTOR Look = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	Look = XMVector3TransformCoord(Look, m);
	Look = XMVector3Normalize(Look);

	XMVECTOR At = (m_Pos + Look);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_View = (XMMatrixLookAtLH(m_Pos, At, Up) * m_Pro);
	D3D::SetMatrix(&m_View);

	XMVECTOR ItemPos = XMVectorSet(0.0f, 0.0f, 4.0f, 0.0f);
	ItemPos = XMVector3TransformCoord(ItemPos, m);
	ItemPos += m_Pos;

	if (m_Item)
	{
		m_Item->Release();
		m_Item = nullptr;
	}

	std::vector<VERTEX> v;
	int s[6] = { 1, 1, 1, 1, 1, 1 };

	float x = floorf(XMVectorGetX(ItemPos));
	x = (x < 0.0f) ? 0.0f : x;
	m_ix = (int)x;

	float y = floorf(XMVectorGetY(ItemPos));
	y = (y < 1.0f) ? 1.0f : y;
	y = (y > 255.0f) ? 255.0f : y;
	m_iy = (int)y;

	float z = floorf(XMVectorGetZ(ItemPos));
	z = (z < 0.0f) ? 0.0f : z;
	m_iz = (int)z;

	Model::InitCube((CUBE_TYPE)m_i, x, y, z, s, &v);

	XMMATRIX sc = XMMatrixScaling(1.001f, 1.001f, 1.001f);
	for (unsigned int i = 0; i < v.size(); i++)
	{
		XMVECTOR xmv = XMVectorSet(v[i].x, v[i].y, v[i].z, 1.0f);
		xmv = XMVector3Transform(xmv, sc);
		v[i].x = XMVectorGetX(xmv); v[i].y = XMVectorGetY(xmv);
		v[i].z = XMVectorGetZ(xmv);
	}

	D3D::InitBuffer(&v[0], (v.size() * sizeof(VERTEX)), &m_Item);
	Map::OnFrame(XMVectorGetX(m_Pos), XMVectorGetZ(m_Pos));
}

void SDemo::Render()
{
	Map::Render();
	if (m_Item) D3D::Render(m_Item);
}

void SDemo::OnKey(long long dt)
{
	const float s = 0.002f;

	if (GetAsyncKeyState('W')) {
		m_mz += s;
	}
	if (GetAsyncKeyState('A')) {
		m_mx -= s;
	}
	if (GetAsyncKeyState('S')) {
		m_mz -= s;
	}
	if (GetAsyncKeyState('D')) {
		m_mx += s;
	}
}

void SDemo::OnKeyLimited(unsigned int Key)
{
	if (Key == 'Q') {
		m_i--;
		m_i = (m_i < 0) ? 0 : m_i;
	}
	else if (Key == 'E') {
		m_i++;
		m_i = (m_i >= CUBE_TYPE_COUNT)
			? (CUBE_TYPE_COUNT - 1) : m_i;
	}
}

void SDemo::OnMouseButton(short Type)
{
	if (Type == MOUSE_LBUTTON && m_i >= CUBE_TYPE_DUMMY) {
		Map::SetCube((CUBE_TYPE)m_i, m_ix, m_iy, m_iz);
	}

	if (Type == MOUSE_RBUTTON) {
		Map::SetCube(CUBE_TYPE_NONE, m_ix, m_iy, m_iz);
	}
}

void SDemo::OnMouseMove(long dx, long dy)
{
	m_cx += (static_cast<float>(dx) * 0.001f);
	m_cy += (static_cast<float>(dy) * 0.001f);

	m_cx = (m_cx >= (2.0f * MATH_PI)) ? 0.0f : m_cx;
	m_cx = (m_cx <= (-2.0f * MATH_PI)) ? 0.0f : m_cx;

	m_cy = (m_cy >= 1.0f) ? 1.0f : m_cy;
	m_cy = (m_cy <= -1.0f) ? -1.0f : m_cy;
}

void SDemo::End()
{
	Map::End();
	if (m_Item) m_Item->Release();
}