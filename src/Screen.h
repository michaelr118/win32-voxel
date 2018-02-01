#pragma once
#define MOUSE_LBUTTON 0
#define MOUSE_RBUTTON 1

class Screen
{
public:
	virtual void init() = 0;
	virtual void OnFrame() = 0;
	virtual void Render() = 0;
	virtual void OnKey(long long dt) = 0;
	virtual void OnKeyLimited(unsigned int Key) = 0;
	virtual void OnMouseButton(short Type) = 0;
	virtual void OnMouseMove(long dx, long dy) = 0;
	virtual void End() = 0;
};