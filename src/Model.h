#pragma once
#include <vector>
#include "igi.h"
#include "Vertex.h"

namespace Model
{
	void InitCube(CUBE_TYPE Type, float x, float y, float z, int s[6],
		std::vector<VERTEX> *dst);
}