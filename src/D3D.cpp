#include "D3D.h"
#include <d3dcompiler.h>
#include <DDSTextureLoader.h>
#include "Vertex.h"

unsigned int D3D_XSCREEN = 0;
unsigned int D3D_YSCREEN = 0;

#define MEMBER static
MEMBER ID3D11Device *m_D3DDevice = nullptr;
MEMBER ID3D11DeviceContext *m_DC = nullptr;
MEMBER IDXGISwapChain *m_SC = nullptr;

MEMBER ID3D11RenderTargetView *m_BB = nullptr;
MEMBER ID3D11DepthStencilView *m_ZB = nullptr;

MEMBER ID3D11VertexShader *m_VS = nullptr;
MEMBER ID3D11InputLayout *m_InputLayout = nullptr;
MEMBER ID3D11PixelShader *m_PS = nullptr;

MEMBER ID3D11Buffer *m_CB = nullptr;

MEMBER ID3D11ShaderResourceView *m_T2D = nullptr;

namespace D3D
{
	void init(HWND Wnd, unsigned int W, unsigned int H)
	{
		D3D_XSCREEN = W;
		D3D_YSCREEN = H;

		const D3D_FEATURE_LEVEL FL[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_9_3
		};
		const unsigned int n = ARRAYSIZE(FL);

		const DXGI_RATIONAL RR = { 60, 1 };

		const DXGI_MODE_DESC DisplayMode = {
			W, H, RR, DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			DXGI_MODE_SCALING_UNSPECIFIED };

		const DXGI_SAMPLE_DESC AA = { 1, 0 };

		const DXGI_SWAP_CHAIN_DESC SCD = {
			DisplayMode, AA, DXGI_USAGE_RENDER_TARGET_OUTPUT,
			1, Wnd, TRUE, DXGI_SWAP_EFFECT_DISCARD,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH };

		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
			NULL, NULL, FL, n, D3D11_SDK_VERSION,
			&SCD, &m_SC, &m_D3DDevice, nullptr, &m_DC);

		IDXGIFactory *pMWA = nullptr;
		m_SC->GetParent(__uuidof(IDXGIFactory), (void**)&pMWA);

		pMWA->MakeWindowAssociation(Wnd, DXGI_MWA_NO_ALT_ENTER);
		pMWA->Release();
		pMWA = nullptr;

		ID3D11Texture2D *pT2D = nullptr;

		m_SC->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pT2D);
		m_D3DDevice->CreateRenderTargetView(pT2D, nullptr, &m_BB);

		pT2D->Release();
		pT2D = nullptr;

		const D3D11_TEXTURE2D_DESC TD = { W, H, 1, 1, DXGI_FORMAT_D32_FLOAT,
			AA, D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL, 0, 0 };

		D3D11_DEPTH_STENCIL_VIEW_DESC DSD;
		ZeroMemory(&DSD, sizeof(DSD));
		DSD.Format = DXGI_FORMAT_D32_FLOAT;
		DSD.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

		m_D3DDevice->CreateTexture2D(&TD, nullptr, &pT2D);
		m_D3DDevice->CreateDepthStencilView(pT2D, &DSD, &m_ZB);

		pT2D->Release();
		pT2D = nullptr;

		m_DC->OMSetRenderTargets(1, &m_BB, m_ZB);

		const D3D11_VIEWPORT Viewport = { 0.0f, 0.0f,
			static_cast<FLOAT>(W), static_cast<FLOAT>(H),
			0.0f, 1.0f };

		m_DC->RSSetViewports(1, &Viewport);

		ID3DBlob *cso = nullptr;

		D3DReadFileToBlob(L"content\\cso\\Vertex.cso", &cso);
		m_D3DDevice->CreateVertexShader(cso->GetBufferPointer(),
			cso->GetBufferSize(), nullptr, &m_VS);
		m_DC->VSSetShader(m_VS, nullptr, 0);

		const D3D11_INPUT_ELEMENT_DESC IED[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		m_D3DDevice->CreateInputLayout(IED, 2, cso->GetBufferPointer(),
			cso->GetBufferSize(), &m_InputLayout);
		m_DC->IASetInputLayout(m_InputLayout);

		cso->Release();
		cso = nullptr;

		D3DReadFileToBlob(L"content\\cso\\Pixel.cso", &cso);
		m_D3DDevice->CreatePixelShader(cso->GetBufferPointer(),
			cso->GetBufferSize(), nullptr, &m_PS);
		m_DC->PSSetShader(m_PS, nullptr, 0);

		cso->Release();
		cso = nullptr;

		const D3D11_BUFFER_DESC BD = { 64, D3D11_USAGE_DEFAULT,
			D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };

		m_D3DDevice->CreateBuffer(&BD, nullptr, &m_CB);
		m_DC->VSSetConstantBuffers(0, 1, &m_CB);

		CreateDDSTextureFromFile(m_D3DDevice, L"content\\dds\\0.dds",
			nullptr, &m_T2D);
		m_DC->PSSetShaderResources(0, 1, &m_T2D);

		ID3D11SamplerState *Sampler = nullptr;
		const D3D11_SAMPLER_DESC sd = {
			D3D11_FILTER_MIN_MAG_MIP_POINT,
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_CLAMP,
			0.0f, 0, D3D11_COMPARISON_NEVER,
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			0.0f, FLT_MAX };

		m_D3DDevice->CreateSamplerState(&sd, &Sampler);
		m_DC->PSSetSamplers(0, 1, &Sampler);

		Sampler->Release();
		Sampler = nullptr;

		ID3D11BlendState *Blender = nullptr;

		D3D11_BLEND_DESC BlendDesc;
		ZeroMemory(&BlendDesc, sizeof(BlendDesc));

		BlendDesc.RenderTarget[0].BlendEnable = TRUE;
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;

		m_D3DDevice->CreateBlendState(&BlendDesc, &Blender);
		m_DC->OMSetBlendState(Blender, 0, 0xFFFFFFFF);
		Blender->Release();
		Blender = nullptr;

		m_DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void BeginRender()
	{
		const FLOAT c[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_DC->ClearRenderTargetView(m_BB, c);
		m_DC->ClearDepthStencilView(m_ZB, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void Render(ID3D11Buffer *b)
	{
		D3D11_BUFFER_DESC BD = { 0 };
		b->GetDesc(&BD);
		const UINT count = (BD.ByteWidth / sizeof(VERTEX));
		const UINT s = sizeof(VERTEX);
		const UINT o = 0;
		m_DC->IASetVertexBuffers(0, 1, &b, &s, &o);
		m_DC->Draw(count, 0);
	}

	void EndScene()
	{
		m_SC->Present(0, 0);
	}

	void SetMatrix(XMMATRIX *Matrix)
	{
		m_DC->UpdateSubresource(m_CB, 0, nullptr, Matrix, 0, 0);
	}

	void InitBuffer(void *d, UINT s, ID3D11Buffer **dst)
	{
		const D3D11_BUFFER_DESC BD = { s,
			D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE, 0, 0 };

		const D3D11_SUBRESOURCE_DATA p = { d, 0, 0 };
		m_D3DDevice->CreateBuffer(&BD, &p, dst);
	}

	void End()
	{
		m_T2D->Release();
		m_CB->Release();
		m_PS->Release();
		m_InputLayout->Release();
		m_VS->Release();
		m_ZB->Release();
		m_BB->Release();
		m_SC->Release();
		m_DC->Release();
		m_D3DDevice->Release();
	}
}