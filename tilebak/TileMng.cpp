#include "TileMng.h"
#include "TilesetID.h"
#include "Engine.h"
#include <gl/gl.h>

#include "prof.h"

extern Engine *eng;

void TileMng::Free()
{
	delete[] tiles;
	tiles = 0;
	num = 0;
}

void TileMng::FromFile(FILE *fp, int len, bool detectSize)
{
	Free();

	num = len/29;
	tiles = new Tile[num];

#ifndef NDEBUG
	printf("Tiles: %d\n", num);
#endif

	//bool detectSize = true; // cam-lock via tiles
	if (eng->render->cam.maxX || eng->render->cam.maxY)
		detectSize = false;

	float maxHeight = 0;
	float maxWidth = 0;

	for (int i=0; i<num; i++)
	{
		tiles[i].sub.t = TilesetID::GetTexture(fgetc(fp));
		fread(&tiles[i].sub.tx, sizeof(int), 1, fp);
		fread(&tiles[i].sub.ty, sizeof(int), 1, fp);
		fread(&tiles[i].sub.w, sizeof(int), 1, fp);
		fread(&tiles[i].sub.h, sizeof(int), 1, fp);
		
		int depth;
		fread(&depth, sizeof(int), 1, fp);

		int xx,yy;
		fread(&xx, sizeof(int), 1, fp);
		fread(&yy, sizeof(int), 1, fp);
		tiles[i].x = (float)xx;
		tiles[i].y = (float)yy;
		tiles[i].depth = depth;
		//printf("Tile %d: (%d,%d) (%d,%d)\n", i, xx, yy, tiles[i].sub.tx, tiles[i].sub.ty);

		tiles[i].flags = TFLAG_BLOCK_N /*| TFLAG_BLOCK_W | TFLAG_BLOCK_E | TFLAG_SLOPE_W*/;

		if (tiles[i].y+tiles[i].sub.h > maxHeight)
			maxHeight = (float)tiles[i].y+tiles[i].sub.h;
	}
	if (detectSize) { // lock camera
		if (maxWidth)
			eng->render->cam.maxX = (float)maxWidth - 640.0f;
		if (maxHeight)
			eng->render->cam.maxY = (float)maxHeight - 480.0f;
	}
}

void TileMng::Draw(int depth)
{
	Prof(TileMng_Draw);

	for (int i=0; i<num; i++)
	{
		if (tiles[i].depth == depth)
			eng->render->DrawSubImage(&tiles[i].sub, tiles[i].x, tiles[i].y);
	}

	/*
	eng->render->lfBeginQuads();
	eng->render->SetColor(255,255,0,100);
	for (int i=0; i<num; i++)
		eng->render->lfDrawQuad(tiles[i].x, tiles[i].y, (float)tiles[i].sub.w, (float)tiles[i].sub.h);
	eng->render->lfEndQuads();
	*/
}

int TileMng::Collision(CollisionObject &col, CollisionObject &actualPos, int flags)
{
#if 0
	for (int i=0; i<num; i++)
	{
		if ((tiles[i].flags & flags) &&
			tiles[i].x < col.x+col.w && // '<='?
			tiles[i].y < col.y+col.h &&
			tiles[i].x > col.x-tiles[i].sub.w &&
			tiles[i].y > col.y-tiles[i].sub.h)
		{
			// fix this code later
			int ret = tiles[i].flags & flags;
			if (flags & TFLAG_BLOCK_N && actualPos.y+actualPos.h > tiles[i].y) // '>' or '>='?
				ret &= ~TFLAG_BLOCK_N; /* we must come from above to trigger this */
			if (flags & TFLAG_BLOCK_S && actualPos.y < tiles[i].y+tiles[i].sub.h)
				ret &= ~TFLAG_BLOCK_S;

			if (flags & TFLAG_BLOCK_W && actualPos.x+actualPos.w > tiles[i].x)
				ret &= ~TFLAG_BLOCK_W;
			if (flags & TFLAG_BLOCK_E && actualPos.x < tiles[i].x+tiles[i].sub.w)
				ret &= ~TFLAG_BLOCK_E;

			/*
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			for i under total width, keep adding 32 to width
				for i under total width, keep adding 32 to height
					check against every solid contained within the tile here
					*/

			if (ret)
				return ret;
		}
	}
#endif
	return 0;
}
