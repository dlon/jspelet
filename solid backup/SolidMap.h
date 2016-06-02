#pragma once
#ifndef SOLIDMAP_H
#define SOLIDMAP_H

#include <vector>
#include "CollisionObject.h"
#include "Vector2D.h"

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

class SolidMap
{
	struct SolidO
	{
		int x;
		int y;
		int flags;
	};
	std::vector<SolidO> solids;
public:
	void FromFile(FILE *fp, int len);
	int Collision(CollisionObject &col, CollisionObject &actualPos, int flags); // return = (flags & 'flags') & matches
				// col = desired position, actualPos = current pos
};

#endif // SOLIDMAP_H
