#include "SolidMap.h"
#include "MapMng.h"

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

void SolidMap::Clear()
{
	for (unsigned x=0; x<SREGION_ARRAY_W; x++)
		for (unsigned y=0; y<SREGION_ARRAY_H; y++)
			solidRegions[x][y].clear();
}

void SolidMap::FromFile(FILE *fp, int len)
{
	for (unsigned x=0; x<SREGION_ARRAY_W; x++)
		for (unsigned y=0; y<SREGION_ARRAY_H; y++)
			solidRegions[x][y].clear();

	//int se=0,sw=0,n=0,e=0,s=0,w=0,im=0;

	for (int i=0; i<len/12; i++)
	{
		int x,y;
		int flags;
		fread(&x, sizeof(int), 1, fp);
		fread(&y, sizeof(int), 1, fp);
		fread(&flags, sizeof(int), 1, fp);

		// find group
		int cx = (x - x % SREGION_W) / SREGION_W;
		int cy = (y - y % SREGION_H) / SREGION_H;
		solidRegions[cx][cy].push_back(SolidContainer(x,y,flags));

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

	// particle emitter
	//ParticleEmitter *em = mapMng.partMng.GetEmitter("waterBubbles");
	//if (!em)
	//	em = mapMng.partMng.NewEmitter("waterBubbles", 0, 0, 50, &waterProp);
}

int SolidMap::Collision(CollisionObject &col, CollisionObject &actualPos, int flags)
{
	// Detect solid region (huge optimization)
	std::vector<SolidContainer>* regions[4];

	if (col.GetX()+col.GetW() < 0 || col.GetY()+col.GetH() < 0)
		return 0; // outside map
	if (col.GetX()+col.GetW() >= SREGION_ARRAY_W*SREGION_W || col.GetY()+col.GetH() >= SREGION_ARRAY_H*SREGION_H) {
#ifndef NDEBUG
		printf("Collision outside of map!\n");
#endif
		return 0; // outside grid: to prevent crashes in the unlikely event that it'll happen
	}

	int cx,cy;
	cx = ((int)col.GetX() - (int)col.GetX() % SREGION_W) / SREGION_W;
	cy = ((int)col.GetY() - (int)col.GetY() % SREGION_H) / SREGION_H;
	regions[0] = &solidRegions[cx][cy]; // top-left
	cx = ((int)(col.GetX()+col.GetW()) - (int)(col.GetX()+col.GetW()) % SREGION_W) / SREGION_W;
	cy = ((int)col.GetY() - (int)col.GetY() % SREGION_H) / SREGION_H;
	regions[1] = &solidRegions[cx][cy]; // top-right
	cx = ((int)col.GetX() - (int)col.GetX() % SREGION_W) / SREGION_W;
	cy = ((int)(col.GetY()+col.GetH()) - (int)(col.GetY()+col.GetH())% SREGION_H) / SREGION_H;
	regions[2] = &solidRegions[cx][cy]; // bottom-left
	cx = ((int)(col.GetX()+col.GetW()) - (int)(col.GetX()+col.GetW()) % SREGION_W) / SREGION_W;
	cy = ((int)(col.GetY()+col.GetH()) - (int)(col.GetY()+col.GetH())% SREGION_H) / SREGION_H;
	regions[3] = &solidRegions[cx][cy]; // bottom-right

	for (int x=0; x<4; x++) { // remove duplicate regions
		for (int y=0; y<4; y++) {
			if (x != y && regions[x] == regions[y]) {
				regions[y] = 0;
				//printf("region[%d] = NULL\n", y);
			}
		}
	}
	
	//int cx = ((int)col.GetX() - (int)col.GetX() % SREGION_W) / SREGION_W;
	//int cy = ((int)col.GetY() - (int)col.GetY() % SREGION_H) / SREGION_H;
	//std::vector<SolidContainer>& solids = solidRegions[cx][cy];

	for (int xxx=0; xxx<4; xxx++)
	{
		// skip null regions
		if (regions[xxx] == NULL)
			continue;
		std::vector<SolidContainer>& solids = *regions[xxx];

		// test all solids within the region!
		size_t num = solids.size();
		for (unsigned i=0; i<num; i++)
		{
			// AABB check
			if ((solids[i].flags & flags) &&
				solids[i].x < col.GetX()+col.GetW() && // <= not < (nope)
				solids[i].y < col.GetY()+col.GetH() &&
				solids[i].x > col.GetX()-SOLID_W &&
				solids[i].y > col.GetY()-SOLID_H)
			{
				// test for requested direction(s)
				int ret = solids[i].flags & flags;

				bool crHack = false;
				if (ret & TFLAG_BLOCK_N && (actualPos.GetY()+actualPos.GetH() > solids[i].y
					//|| (crHack=(col.GetX()+col.lSlopeOff >= solids[i].x+SOLID_W || col.GetX()+col.GetW()-col.rSlopeOff <= solids[i].x)) // quick fix for corners - FIXME: cuts through corners at times - we "fall through" the East/West border
					))
				{
					ret &= ~TFLAG_BLOCK_N; /* we must come from above to trigger this */
				}

				if (ret & TFLAG_BLOCK_S && actualPos.GetY() < solids[i].y+SOLID_H)
					ret &= ~TFLAG_BLOCK_S;

				if (ret & TFLAG_BLOCK_W && actualPos.GetX()+actualPos.GetW() > solids[i].x)
					ret &= ~TFLAG_BLOCK_W;
				if (ret & TFLAG_BLOCK_E && actualPos.GetX() < solids[i].x+SOLID_W)
					ret &= ~TFLAG_BLOCK_E;
				/*
				// I think I prefer the bug over this
				if (ret & TFLAG_BLOCK_W && actualPos.x+actualPos.w-col.rSlopeOff > solids[i].x)
					ret &= ~TFLAG_BLOCK_W;
				if (ret & TFLAG_BLOCK_E && actualPos.x+col.lSlopeOff < solids[i].x+SOLID_W)
					ret &= ~TFLAG_BLOCK_E;
				*/

				// sloppy slope
				if (ret & TFLAG_SLOPE_E) {
					if (!PointInRightTrng((float)solids[i].x, (float)solids[i].y, col.GetX()+col.lSlopeOff, col.GetY()+col.GetH()))
						ret &= ~TFLAG_SLOPE_E;
				}
				else if (ret & TFLAG_SLOPE_W) {
					if (!PointInLeftTrng((float)solids[i].x, (float)solids[i].y, col.GetX()+col.GetW()-col.rSlopeOff, col.GetY()+col.GetH()))
						ret &= ~TFLAG_SLOPE_W;
				}
				else if (ret & TFLAG_SLOPE_HE) {
					if (!PointInObRightTrng((float)solids[i].x, (float)solids[i].y, col.GetX()+col.lSlopeOff, col.GetY()+col.GetH()))
						ret &= ~TFLAG_SLOPE_HE;
				}
				else if (ret & TFLAG_SLOPE_HW) {
					if (!PointInObLeftTrng((float)solids[i].x, (float)solids[i].y, col.GetX()+col.GetW()-col.rSlopeOff, col.GetY()+col.GetH()))
						ret &= ~TFLAG_SLOPE_HW;
				}

				if (ret)
					return ret;
			}
		}
	}
	return 0;
}
