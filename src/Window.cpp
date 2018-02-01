#include "Window.h"
#include "GSM.h"
#define MESSAGE_HANDLED 0

#define MEMBER static
MEMBER HWND m_Window = NULL;
MEMBER const HINSTANCE m_Application = GetModuleHandle(NULL);

MEMBER bool m_ClipMouse = true;

MEMBER LRESULT CALLBACK OnMessage(HWND Wnd, UINT Message, WPARAM W, LPARAM L);

namespace Window
{
	void init(const wchar_t *Name, int W, int H)
	{
		const WNDCLASS wc = { (CS_HREDRAW | CS_VREDRAW), OnMessage,
			0, 0, m_Application, LoadIcon(NULL, IDI_APPLICATION),
			LoadCursor(NULL, IDC_ARROW), NULL, NULL, L"D3DWindow" };

		const DWORD ws = (WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX);

		RECT wr = { 0, 0, W, H };
		AdjustWindowRect(&wr, ws, FALSE);

		const int w = (wr.right - wr.left);
		const int h = (wr.bottom - wr.top);
		const int x = ((GetSystemMetrics(SM_CXSCREEN) - w) / 2);
		const int y = ((GetSystemMetrics(SM_CYSCREEN) - h) / 2);

		RegisterClass(&wc);
		m_Window = CreateWindow(L"D3DWindow", Name, ws, x, y, w, h,
			NULL, NULL, m_Application, NULL);

		const RAWINPUTDEVICE Mouse = { 0x01, 0x02, NULL, m_Window };
		RegisterRawInputDevices(&Mouse, 1, sizeof(RAWINPUTDEVICE));
		ShowCursor(FALSE);
	}

	void ClipMouse(long long dt)
	{
		if (m_ClipMouse)
		{
			RECT cr;
			GetClientRect(m_Window, &cr);
			ClientToScreen(m_Window, (LPPOINT)&cr);
			ClipCursor(&cr);
			GSM::OnKey(dt);
		}
	}

	HWND GetHandle()
	{
		return m_Window;
	}

	void Show()
	{
		ShowWindow(m_Window, SW_SHOW);
	}

	void Hide()
	{
		ShowWindow(m_Window, SW_HIDE);
	}

	void End()
	{
		DestroyWindow(m_Window);
		UnregisterClass(L"D3DWindow", m_Application);
	}
}

MEMBER LRESULT CALLBACK OnMessage(HWND Wnd, UINT Message, WPARAM W, LPARAM L)
{
	switch (Message)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return MESSAGE_HANDLED;
	}
	case WM_INPUT:
	{
		RAWINPUT Input;
		UINT s = sizeof(RAWINPUT);
		GetRawInputData((HRAWINPUT)L, RID_INPUT, &Input, &s,
			sizeof(RAWINPUTHEADER));

		if (Input.header.dwType == RIM_TYPEMOUSE && m_ClipMouse)
		{
			GSM::OnMouseMove(Input.data.mouse.lLastX,
				Input.data.mouse.lLastY);
		}
		return MESSAGE_HANDLED;
	}
	case WM_KEYDOWN:
	{
		if (W == VK_ESCAPE && m_ClipMouse)
		{
			ShowCursor(TRUE);
			ClipCursor(nullptr);
			m_ClipMouse = false;
		}
		else if (m_ClipMouse)
		{
			GSM::OnKeyLimited(W);
		}
		return MESSAGE_HANDLED;
	}
	case WM_KILLFOCUS:
	{
		if (m_ClipMouse)
		{
			ShowCursor(TRUE);
			ClipCursor(nullptr);
			m_ClipMouse = false;
		}
		return MESSAGE_HANDLED;
	}
	case WM_LBUTTONDOWN:
	{
		if (!m_ClipMouse)
		{
			ShowCursor(FALSE);
			m_ClipMouse = true;
		}
		else
		{
			GSM::OnMouseButton(MOUSE_LBUTTON);
		}
		return MESSAGE_HANDLED;
	}
	case WM_RBUTTONDOWN:
	{
		if (m_ClipMouse)
		{
			GSM::OnMouseButton(MOUSE_RBUTTON);
		}
		return MESSAGE_HANDLED;
	}
	}
	return DefWindowProc(Wnd, Message, W, L);
}