#pragma once
#include "igi.h"

namespace Map
{
	void init(float x, float z);

	void OnFrame(float x, float z);
	void Render();

	CUBE_TYPE CubeTypeAt(int x, int y, int z);

	CUBE_TYPE HitTest(float x, float y, float z, float cx, float cy,
		int *ox, int *oy, int *oz);

	void SetCube(CUBE_TYPE Type, int x, int y, int z);

	void End();
}