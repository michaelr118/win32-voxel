#include "GSM.h"

#define MEMBER static
MEMBER Screen *m_Screen = nullptr;

namespace GSM
{
	void Push(Screen *s)
	{
		if (m_Screen)
		{
			m_Screen->End();
			delete m_Screen;
			m_Screen = nullptr;
		}

		if (s)
		{
			m_Screen = s;
			m_Screen->init();
		}
	}

	void OnFrame()
	{
		if (m_Screen)
		{
			m_Screen->OnFrame();
		}
	}

	void Render()
	{
		if (m_Screen)
		{
			m_Screen->Render();
		}
	}

	void OnKey(long long dt)
	{
		if (m_Screen)
		{
			m_Screen->OnKey(dt);
		}
	}

	void OnKeyLimited(unsigned int Key)
	{
		if (m_Screen)
		{
			m_Screen->OnKeyLimited(Key);
		}
	}

	void OnMouseButton(short Type)
	{
		if (m_Screen)
		{
			m_Screen->OnMouseButton(Type);
		}
	}

	void OnMouseMove(long dx, long dy)
	{
		if (m_Screen)
		{
			m_Screen->OnMouseMove(dx, dy);
		}
	}

	void EndAll()
	{
		if (m_Screen)
		{
			m_Screen->End();
			delete m_Screen;
			m_Screen = nullptr;
		}
	}
}