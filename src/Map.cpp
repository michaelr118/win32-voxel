#include "Map.h"
#include <mutex>
#include <thread>
#include <vector>
#include "D3D.h"
#include "Model.h"

#define MAX_DISTANCE 3
#define MAX_LOAD ((MAX_DISTANCE * 2) * (MAX_DISTANCE * 2)) + 1

#define CHUNK_SIZE 32
#define MAX_HEIGHT 256

#define MEMBER static

struct CHUNK
{
	bool Occupied;
	bool Clean;
	std::mutex mtx;
	int p, q;
	CUBE_TYPE Cube[CHUNK_SIZE][CHUNK_SIZE][MAX_HEIGHT];
	ID3D11Buffer *vb;
};

MEMBER std::vector<std::thread> m_Worker;
MEMBER int m_Loop = 1;
MEMBER CHUNK m_Chunk[MAX_LOAD];
MEMBER float m_x, m_z;

MEMBER void CleanChunk(CHUNK *c);
MEMBER void Expand();
MEMBER CHUNK *FindChunkPQ(int p, int q);
MEMBER CHUNK *FindChunkXZ(int x, int z);
MEMBER CUBE_TYPE FindCubeType(CHUNK *c, int x, int z, int y);

MEMBER void Implode();
MEMBER void InitChunk(int p, int q);

MEMBER void ToLocal(int *x, int *z);

namespace Map
{
	void init(float x, float z)
	{
		m_x = x; m_z = z;

		for (int i = 0; i < MAX_LOAD; i++)
		{
			m_Chunk[i].mtx.lock();
			m_Chunk[i].Occupied = false;
			m_Chunk[i].Clean = false;
			m_Chunk[i].p = m_Chunk[i].q = 0;
			m_Chunk[i].vb = nullptr;
			m_Chunk[i].mtx.unlock();
		}

		m_Worker.push_back(std::thread(Expand));
		m_Worker.push_back(std::thread(Implode));
	}

	void OnFrame(float x, float z)
	{
		m_x = x; m_z = z;
	}

	void Render()
	{
		for (int i = 0; i < MAX_LOAD; i++)
		{
			if (m_Chunk[i].Occupied && m_Chunk[i].Clean && m_Chunk[i].vb)
				D3D::Render(m_Chunk[i].vb);
		}
	}

	CUBE_TYPE CubeTypeAt(int x, int y, int z)
	{
		return FindCubeType(nullptr, x, y, z);
	}

	void SetCube(CUBE_TYPE Type, int x, int y, int z)
	{
		CHUNK *c = FindChunkXZ(x, z);
		if (!c) return;

		ToLocal(&x, &z);
		c->mtx.lock();
		c->Cube[x][z][y] = Type;
		c->mtx.unlock();
		CleanChunk(c);
	}

	void End()
	{
		m_Loop = 0;
		for (unsigned int i = 0; i < m_Worker.size(); i++)
		{
			while (!m_Worker[i].joinable()) {}
			m_Worker[i].join();
		}
		m_Worker.clear();

		for (int i = 0; i < MAX_LOAD; i++)
		{
			CHUNK *c = (m_Chunk + i);
			if (c->Occupied)
			{
				if (c->vb) c->vb->Release();
				c->Occupied = false;
			}
		}
	}
}

MEMBER void CleanChunk(CHUNK *c)
{
	c->mtx.lock();
	c->Clean = false;
	if (c->vb)
	{
		c->vb->Release();
		c->vb = nullptr;
	}

	std::vector<VERTEX> Final;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int y = 0; y < MAX_HEIGHT; y++)
			{
				if (!c->Cube[x][z][y]) continue;

				int s[6] = { 1, 1, 1, 1, 1, 1 };

				s[0] = (z > 0 && c->Cube[x][z - 1][y]) ? 0 : 1;
				s[1] = (x < (CHUNK_SIZE - 1) && c->Cube[x + 1][z][y]) ? 0 : 1;
				s[2] = (z < (CHUNK_SIZE - 1) && c->Cube[x][z + 1][y]) ? 0 : 1;
				s[3] = (x > 0 && c->Cube[x - 1][z][y]) ? 0 : 1;
				s[4] = (y > 0 && c->Cube[x][z][y - 1]) ? 0 : 1;
				s[5] = (y < (MAX_HEIGHT - 1) && c->Cube[x][z][y + 1]) ? 0 : 1;

				std::vector<VERTEX> v;
				Model::InitCube(c->Cube[x][z][y], (float)((c->p * CHUNK_SIZE) + x),
					(float)y, (float)((c->q * CHUNK_SIZE) + z), s, &v);
				Final.insert(Final.end(), v.begin(), v.end());
			}
		}
	}

	D3D::InitBuffer(&Final[0], (Final.size() * sizeof(VERTEX)), &c->vb);
	c->Clean = true;
	c->mtx.unlock();
}

MEMBER void Expand()
{
	while (m_Loop)
	{
		int p = ((int)m_x / CHUNK_SIZE);
		int q = ((int)m_z / CHUNK_SIZE);

		for (int i = (p - MAX_DISTANCE); i <= (p + MAX_DISTANCE); i++)
		{
			float u = (float)(i - p);
			for (int j = (q - MAX_DISTANCE); j <= (q + MAX_DISTANCE); j++)
			{
				float v = (float)(j - q);

				int d = (int)sqrt((u * u) + (v * v));
				if (d > MAX_DISTANCE || i < 0 || j < 0) continue;

				CHUNK *c = FindChunkPQ(i, j);
				if (c) continue;
				InitChunk(i, j);
			}
		}
	}
}

MEMBER CHUNK *FindChunkPQ(int p, int q)
{
	CHUNK *c = nullptr;
	for (int i = 0; i < MAX_LOAD; i++)
	{
		CHUNK *d = (m_Chunk + i);
		d->mtx.lock();
		if (d->Occupied && d->p == p && d->q == q)
		{
			c = d;
			d->mtx.unlock();
			break;
		}
		d->mtx.unlock();
	}
	return c;
}

MEMBER CHUNK *FindChunkXZ(int x, int z)
{
	int p = (x / CHUNK_SIZE);
	int q = (z / CHUNK_SIZE);
	return FindChunkPQ(p, q);
}

MEMBER CUBE_TYPE FindCubeType(CHUNK *c, int x, int z, int y)
{
	CUBE_TYPE t = CUBE_TYPE_NONE;

	int p = (x / CHUNK_SIZE);
	int q = (z / CHUNK_SIZE);
	int u = (x - (p * CHUNK_SIZE));
	int v = (z - (q * CHUNK_SIZE));

	if (!c) c = FindChunkPQ(p, q);
	if (c)
	{
		t = c->Cube[u][v][y];
	}

	return t;
}

MEMBER void Implode()
{
	while (m_Loop)
	{
		int p = ((int)m_x / CHUNK_SIZE);
		int q = ((int)m_z / CHUNK_SIZE);

		for (int i = 0; i < MAX_LOAD; i++)
		{
			CHUNK *c = (m_Chunk + i);
			c->mtx.lock();
			float u = (float)(c->p - p);
			float v = (float)(c->q - q);
			int d = (int)sqrt((u * u) + (v * v));
			if (d > MAX_DISTANCE)
			{
				if (c->vb)
				{
					c->vb->Release();
					c->vb = nullptr;
				}
				c->Occupied = false;
			}
			c->mtx.unlock();
		}
	}
}

MEMBER void InitChunk(int p, int q)
{
	CHUNK *c;
	for (int i = 0; i < MAX_LOAD; i++)
	{
		CHUNK *d = (m_Chunk + i);
		d->mtx.lock();
		if (!d->Occupied)
		{
			c = d;
			break;
		}
		d->mtx.unlock();
	}

	c->p = p; c->q = q;

	ZeroMemory(&c->Cube, (sizeof(CUBE_TYPE) * CHUNK_SIZE * CHUNK_SIZE * MAX_HEIGHT));
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			c->Cube[i][j][0] = CUBE_TYPE_DUMMY;
		}
	}

	std::vector<VERTEX> Final;
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		float x = (float)((p * CHUNK_SIZE) + i);
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			float z = (float)((q * CHUNK_SIZE) + j);
			std::vector<VERTEX> v;
			int s[6] = { 0, 0, 0, 0, 0, 1 };
			Model::InitCube(c->Cube[i][j][0], x, 0.0f, z, s, &v);
			Final.insert(Final.end(), v.begin(), v.end());
		}
	}
	D3D::InitBuffer(&Final[0], (Final.size() * sizeof(VERTEX)), &c->vb);
	c->Clean = true;
	c->Occupied = true;
	c->mtx.unlock();
}

MEMBER void ToLocal(int *x, int *z)
{
	*x = (*x - ((*x / CHUNK_SIZE) * CHUNK_SIZE));
	*z = (*z - ((*z / CHUNK_SIZE) * CHUNK_SIZE));
}