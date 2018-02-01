#pragma once

enum CUBE_TYPE {
	CUBE_TYPE_NONE,
	CUBE_TYPE_FRAME,
	CUBE_TYPE_DUMMY,
	CUBE_TYPE_DIRT,
	CUBE_TYPE_COUNT
};

struct TEX_INDEX {
	unsigned char Row, Col;
};

extern const TEX_INDEX igiCubeIndex[CUBE_TYPE_COUNT][6];