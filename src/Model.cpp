#include "Model.h"
#define ONE_SIXTEENTH 0.0625f
#define MEMBER static

namespace Model
{
	void InitCube(CUBE_TYPE Type, float x, float y, float z, int s[6],
		std::vector<VERTEX> *dst)
	{
		VERTEX Cube[] = {
			// FRONT
			{ POS(0.0f, 0.0f, 0.0f), TEX_UV(0.0f, 0.0625f) },
			{ POS(0.0f, 1.0f, 0.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(1.0f, 0.0f, 0.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(1.0f, 0.0f, 0.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(0.0f, 1.0f, 0.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(1.0f, 1.0f, 0.0f), TEX_UV(0.0625f, 0.0f) },
			// RIGHT
			{ POS(1.0f, 0.0f, 0.0f), TEX_UV(0.0f, 0.0625f) },
			{ POS(1.0f, 1.0f, 0.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(1.0f, 0.0f, 1.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(1.0f, 0.0f, 1.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(1.0f, 1.0f, 0.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(1.0f, 1.0f, 1.0f), TEX_UV(0.0625f, 0.0f) },
			// BACK
			{ POS(1.0f, 0.0f, 1.0f), TEX_UV(0.0f, 0.0625f) },
			{ POS(1.0f, 1.0f, 1.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(0.0f, 0.0f, 1.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(0.0f, 0.0f, 1.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(1.0f, 1.0f, 1.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(0.0f, 1.0f, 1.0f), TEX_UV(0.0625f, 0.0f) },
			// LEFT
			{ POS(0.0f, 0.0f, 1.0f), TEX_UV(0.0f, 0.0625f) },
			{ POS(0.0f, 1.0f, 1.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(0.0f, 0.0f, 0.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(0.0f, 0.0f, 0.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(0.0f, 1.0f, 1.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(0.0f, 1.0f, 0.0f), TEX_UV(0.0625f, 0.0f) },
			// BOTTOM
			{ POS(0.0f, 0.0f, 1.0f), TEX_UV(0.0f, 0.0625f) },
			{ POS(0.0f, 0.0f, 0.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(1.0f, 0.0f, 1.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(1.0f, 0.0f, 1.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(0.0f, 0.0f, 0.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(1.0f, 0.0f, 0.0f), TEX_UV(0.0625f, 0.0f) },
			// TOP
			{ POS(0.0f, 1.0f, 0.0f), TEX_UV(0.0f, 0.0625f) },
			{ POS(0.0f, 1.0f, 1.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(1.0f, 1.0f, 0.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(1.0f, 1.0f, 0.0f), TEX_UV(0.0625f, 0.0625f) },
			{ POS(0.0f, 1.0f, 1.0f), TEX_UV(0.0f, 0.0f) },
			{ POS(1.0f, 1.0f, 1.0f), TEX_UV(0.0625f, 0.0f) }
		};

		for (int i = 0; i < 36; i++)
		{
			const int j = (i / 6);
			if (!s[j]) continue;

			Cube[i].x += x; Cube[i].y += y;
			Cube[i].z += z;

			Cube[i].u += ((float)igiCubeIndex[Type][j].Col
				* ONE_SIXTEENTH);
			Cube[i].v += ((float)igiCubeIndex[Type][j].Row
				* ONE_SIXTEENTH);

			dst->push_back(Cube[i]);
		}
	}
}