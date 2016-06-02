// TODO: flyweight tiles

#pragma once
#ifndef TILEMNG_H
#define TILEMNG_H

#include "Renderer.h"
#include "Entity.h"
#include <stdio.h>
#include "CollisionObject.h"
#include "Vector2D.h"
#include <vector>
#include <map>

//#define TILEMNG_USE_DISPLAY_LISTS

#ifndef SOLIDMAP_H
enum
{
	TFLAG_BLOCK_N		= 0x01,	// north
	TFLAG_BLOCK_E		= 0x02,	// east
	TFLAG_BLOCK_W		= 0x04,	// west
	TFLAG_BLOCK_S		= 0x08,	// south
	TFLAG_SLOPE_E		= 0x10,
	TFLAG_SLOPE_W		= 0x20,
	TFLAG_SLOPE_HE		= 0x40,	// half slope
	TFLAG_SLOPE_HW		= 0x80,
	TFLAG_IMPENETRABLE	= 0x0100
};
#endif // SOLIDMAP_H

struct Tile {
	SubImage	sub;
	float		x,y;
};

class TileMng
{
	std::vector<Tile> tileV[3];
	int num;
#ifdef TILEMNG_USE_DISPLAY_LISTS
	unsigned tileDList[3];
	void GenDisplayList();
#endif
public:
	~TileMng() { Free(); }

	void FromFile(FILE *fp, int len, bool detectSize);
	void Draw(int depth);
	void Free();
};

#endif // TILEMNG_H
