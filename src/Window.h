#pragma once
#include <Windows.h>

namespace Window
{
	void init(const wchar_t *Name, int W, int H);

	void ClipMouse(long long dt);
	HWND GetHandle();

	void Show();
	void Hide();

	void End();
}