#pragma once
#include "Screen.h"

namespace GSM
{
	void Push(Screen *);

	void OnFrame();
	void Render();

	void OnKey(long long dt);
	void OnKeyLimited(unsigned int Key);
	void OnMouseButton(short Type);
	void OnMouseMove(long dx, long dy);

	void EndAll();
}