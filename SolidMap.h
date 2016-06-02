#pragma once
#ifndef SOLIDMAP_H
#define SOLIDMAP_H

#include <vector>
#include "CollisionObject.h"
#include "Vector2D.h"

class MapMng;

#ifndef TILEMNG_H
enum
{
	TFLAG_BLOCK_N		= 0x01,	// north
	TFLAG_BLOCK_E		= 0x02,	// east
	TFLAG_BLOCK_W		= 0x04,	// west
	TFLAG_BLOCK_S		= 0x08,	// south
	TFLAG_SLOPE_E		= 0x10, // right=down, left=up
	TFLAG_SLOPE_W		= 0x20,
	TFLAG_SLOPE_HE		= 0x40,	// half slope
	TFLAG_SLOPE_HW		= 0x80,
	TFLAG_IMPENETRABLE	= 0x0100,
	TFLAG_WATER			= 0x0200
};
#endif // TILEMNG_H

#define SOLID_W 32
#define SOLID_H 32

/*
#define SREGION_W 512
#define SREGION_H 512
#define SREGION_ARRAY_W 25
#define SREGION_ARRAY_H 15
*/
// Quick fix'd... FIXME: Too much memory usage?
#define SREGION_W 512
#define SREGION_H 512
#define SREGION_ARRAY_W 60
#define SREGION_ARRAY_H 20

/*
#define SREGION_W 320
#define SREGION_H 320
#define SREGION_ARRAY_W 120
#define SREGION_ARRAY_H 40
*/

struct SolidContainer {
	int x;
	int y;
	int flags;

	SolidContainer(int _x,int _y, int _flags) : x(_x), y(_y), flags(_flags) {}
};

class SolidMap
{
	// large boundary area: speed optimization that had to be done
	// Memory: 4 * SREGION_ARRAY_W*SREGION_ARRAY_H*3 (excluding vector sizes)
	std::vector<SolidContainer> solidRegions[SREGION_ARRAY_W][SREGION_ARRAY_H];
	MapMng &mapMng;
public:
	void FromFile(FILE *fp, int len);
	void Clear();
	int Collision(CollisionObject &col, CollisionObject &actualPos, int flags); // return = (flags & 'flags') & matches
				// col = desired position, actualPos = current pos

	SolidMap(MapMng &mapMng) : mapMng(mapMng) {}
};

#endif // SOLIDMAP_H
