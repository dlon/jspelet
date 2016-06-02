#include "SolidMap.h"

#include "prof.h"

bool PointInRightTrng(float tx, float ty, float x, float y) {
	return (x - tx) - (y - ty) < 0; // hypo
}
bool PointInLeftTrng(float tx, float ty, float x, float y) {
	//return (tx - x) - (y - ty) < 0; // hypo
	return 32+(tx - x) - (y - ty) < 0; // hypo
}

bool PointInObRightTrng(float tx, float ty, float x, float y) {
	if (y-ty < 16) return false;
	return (x - tx) - 2*(y - ty - 16) < 0; // hypo
}
bool PointInObLeftTrng(float tx, float ty, float x, float y) {
	if (y-ty < 16) return false;
	return 32+(tx - x) - 2*(y - ty - 16) < 0; // hypo
}

void SolidMap::FromFile(FILE *fp, int len)
{
	solids.clear(); // free

	unsigned num = len/12;
	solids.reserve(num);

	//int se=0,sw=0,n=0,e=0,s=0,w=0,im=0;

	for (unsigned i=0; i<num; i++)
	{
		SolidO temp;
		fread(&temp.x, sizeof(int), 1, fp);
		fread(&temp.y, sizeof(int), 1, fp);
		fread(&temp.flags, sizeof(int), 1, fp);
		solids.push_back(temp);

		/*
		if (temp.flags & TFLAG_SLOPE_E) se++;
		if (temp.flags & TFLAG_SLOPE_W) sw++;
		if (temp.flags & TFLAG_BLOCK_N) n++;
		if (temp.flags & TFLAG_BLOCK_E) e++;
		if (temp.flags & TFLAG_BLOCK_W) w++;
		if (temp.flags & TFLAG_BLOCK_S) s++;
		if (temp.flags & TFLAG_IMPENETRABLE) im++;
		*/
	}
	//printf("im: %d\ne: %d\nw: %d\nn: %d\ns: %d\nsw: %d\nse: %d\n", im,e,w,n,s,sw,se);
}

int SolidMap::Collision(CollisionObject &col, CollisionObject &actualPos, int flags)
{
	Prof(SolidMap_Collision);

	// todo: we need to implement some larger bounds check to avoid the massive amount of solid checks (exponential w/ increasing entities)
	// or: less solids...

	size_t num = solids.size();
	for (unsigned i=0; i<num; i++)
	{
		Prof(SolidMap_Collision_AABB);

		// AABB check
		if ((solids[i].flags & flags) &&
			solids[i].x < col.x+col.w && // <= not < (nope)
			solids[i].y < col.y+col.h &&
			solids[i].x > col.x-SOLID_W &&
			solids[i].y > col.y-SOLID_H)
		{
			Prof(SolidMap_Collision_SubRoutines);

			// test for requested direction(s)
			int ret = solids[i].flags & flags;
			
			if (ret & TFLAG_BLOCK_N && (actualPos.y+actualPos.h > solids[i].y
				/*|| (col.x+col.lSlopeOff >= solids[i].x+SOLID_W || col.x+col.w-col.rSlopeOff <= solids[i].x)*/ )) // quick fix for corners - FIXME: cuts through corners at times - pixel-perfect? :|
				ret &= ~TFLAG_BLOCK_N; /* we must come from above to trigger this */

			if (ret & TFLAG_BLOCK_S && actualPos.y < solids[i].y+SOLID_H)
				ret &= ~TFLAG_BLOCK_S;

			if (ret & TFLAG_BLOCK_W && actualPos.x+actualPos.w > solids[i].x)
				ret &= ~TFLAG_BLOCK_W;
			if (ret & TFLAG_BLOCK_E && actualPos.x < solids[i].x+SOLID_W)
				ret &= ~TFLAG_BLOCK_E;

			// sloppy slope
			if (ret & TFLAG_SLOPE_E)
			{
				// test slope with a centered point

				/*if (actualPos.y+actualPos.h > solids[i].y) // land from above, otherwise jump through
					ret &= ~TFLAG_SLOPE_E;
				else*/
				{
					if (!PointInRightTrng(solids[i].x, solids[i].y, col.x+col.lSlopeOff, col.y+col.h))
						ret &= ~TFLAG_SLOPE_E;
				}
			}
			else if (ret & TFLAG_SLOPE_W)
			{
				if (!PointInLeftTrng(solids[i].x, solids[i].y, col.x+col.w-col.rSlopeOff, col.y+col.h))
					ret &= ~TFLAG_SLOPE_W;
			}
			else if (ret & TFLAG_SLOPE_HE)
			{
				if (!PointInObRightTrng(solids[i].x, solids[i].y, col.x+col.lSlopeOff, col.y+col.h))
					ret &= ~TFLAG_SLOPE_HE;
			}
			else if (ret & TFLAG_SLOPE_HW)
			{
				if (!PointInObLeftTrng(solids[i].x, solids[i].y, col.x+col.w-col.rSlopeOff, col.y+col.h))
					ret &= ~TFLAG_SLOPE_HW;
			}

			if (ret)
				return ret;
		}
	}
	return 0;
}
