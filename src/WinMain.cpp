#include <chrono>
#include <Windows.h>
#include "D3D.h"
#include "GSM.h"
#include "SDemo.h"
#include "Window.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int ExitCode = 0;

	Window::init(L"Spire Client", 900, 600);
	D3D::init(Window::GetHandle(), 900, 600);
	GSM::Push(new SDemo);
	Window::Show();

	std::chrono::high_resolution_clock hrc;
	std::chrono::high_resolution_clock::time_point punch = hrc.now();

	bool Loop = true;
	while (Loop)
	{
		MSG Message = { 0 };
		while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT)
			{
				ExitCode = Message.wParam;
				Loop = false;
			}

			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		std::chrono::high_resolution_clock::time_point now = hrc.now();
		std::chrono::milliseconds ms = std::chrono::duration_cast
			<std::chrono::milliseconds>(now - punch);

		punch = now;
		long long dt = ms.count();

		Window::ClipMouse(dt);
		GSM::OnFrame();
		D3D::BeginRender();
		GSM::Render();
		D3D::EndScene();
	}

	Window::Hide();
	GSM::EndAll();
	D3D::End();
	Window::End();

	return ExitCode;
}