#include "TileMng.h"
#include "TilesetID.h"
#include "Engine.h"
#include <gl/gl.h>
#include <algorithm>

#include "prof.h"

extern Engine *eng;

void TileMng::Free()
{
#ifdef TILEMNG_USE_DISPLAY_LISTS
	if (tileV[0].size())
		glDeleteLists(tileDList[0], 1);
	if (tileV[1].size())
		glDeleteLists(tileDList[1], 1);
	if (tileV[2].size())
		glDeleteLists(tileDList[2], 1);
#endif

	tileV[0].clear();
	tileV[1].clear();
	tileV[2].clear();
	num = 0;
}

bool SortByTexture(const Tile &t1, const Tile &t2)
{
	return t1.sub.t < t2.sub.t;
}

#ifdef TILEMNG_USE_DISPLAY_LISTS
void TileMng::GenDisplayList()
{
	// create display lists for tiles
	for (int depth=0; depth<3; depth++)
	{
		if (!tileV[depth].size())
			continue;

		tileDList[depth] = glGenLists(1);
		glNewList(tileDList[depth], GL_COMPILE);

		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);

		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(eng->render->textureType);
#if 0
		Texture *tt = tileV[depth][0].sub.t;
		glBindTexture(eng->render->textureType, tt->reserved);
		
		if (eng->render->rectExt) { // move this if-statement outside of loop if it helps
			glBegin(GL_QUADS);
			
			for (unsigned int i=0; i<tileV[depth].size(); i++) {
				if (tileV[depth][i].sub.t != tt) {
					glEnd();
					
					tt = tileV[depth][i].sub.t;
					glBindTexture(eng->render->textureType, tt->reserved);
					
					glBegin(GL_QUADS);
				}

				SubImage *s = &tileV[depth][i].sub;
				float &x = tileV[depth][i].x;
				float &y = tileV[depth][i].y;

				// draw tile
				glTexCoord2i(s->tx, s->ty);
				glVertex2f(x, y);
				glTexCoord2i(s->tx, s->ty+s->h);
				glVertex2f(x, y+s->h);
				glTexCoord2i(s->tx+s->w, s->ty+s->h);
				glVertex2f(x+s->w, y+s->h);
				glTexCoord2i(s->tx+s->w, s->ty);
				glVertex2f(x+s->w, y);
			}
			glEnd();
		}
		else {
			glBegin(GL_QUADS);
			
			for (unsigned int i=0; i<tileV[depth].size(); i++) {
				if (tileV[depth][i].sub.t != tt) {
					glEnd();
					
					tt = tileV[depth][i].sub.t;
					glBindTexture(eng->render->textureType, tt->reserved);
					
					glBegin(GL_QUADS);
				}

				SubImage *s = &tileV[depth][i].sub;
				float &x = tileV[depth][i].x;
				float &y = tileV[depth][i].y;

				// draw tile
				float xw = float(s->t->w + s->t->paddingW);
				float yw = float(s->t->h + s->t->paddingH);

				glTexCoord2f(s->tx / xw, s->ty / yw);
				glVertex2f(x, y);
				glTexCoord2f(s->tx / xw, (s->ty+s->h) / yw);
				glVertex2f(x, y+s->h);
				glTexCoord2f((s->tx+s->w) / xw, (s->ty+s->h) / yw);
				glVertex2f(x+s->w, y+s->h);
				glTexCoord2f((s->tx+s->w) / xw, s->ty / yw);
				glVertex2f(x+s->w, y);
			}
			glEnd();
		}
#endif
		Texture *tt = 0;
		
		if (eng->render->rectExt) { // move this if-statement outside of loop if it helps
			for (unsigned int i=0; i<tileV[depth].size(); i++) {
				if (tileV[depth][i].sub.t != tt) {
					if (tt)
						glEnd();
					
					tt = tileV[depth][i].sub.t;
					glEnable(eng->render->textureType);
					glBindTexture(eng->render->textureType, tt->reserved);
					
					glBegin(GL_QUADS);
				}

				SubImage *s = &tileV[depth][i].sub;
				float &x = tileV[depth][i].x;
				float &y = tileV[depth][i].y;

				// draw tile
				glTexCoord2i(s->tx, s->ty);
				glVertex2f(x, y);
				glTexCoord2i(s->tx, s->ty+s->h);
				glVertex2f(x, y+s->h);
				glTexCoord2i(s->tx+s->w, s->ty+s->h);
				glVertex2f(x+s->w, y+s->h);
				glTexCoord2i(s->tx+s->w, s->ty);
				glVertex2f(x+s->w, y);
			}
			if (tt)
				glEnd();
		}
		else {
			for (unsigned int i=0; i<tileV[depth].size(); i++) {
				if (tileV[depth][i].sub.t != tt) {
					if (tt)
						glEnd();
					
					tt = tileV[depth][i].sub.t;
					glEnable(eng->render->textureType);
					glBindTexture(eng->render->textureType, tt->reserved);
					
					glBegin(GL_QUADS);
				}

				SubImage *s = &tileV[depth][i].sub;
				float &x = tileV[depth][i].x;
				float &y = tileV[depth][i].y;

				// draw tile
				float xw = float(s->t->w + s->t->paddingW);
				float yw = float(s->t->h + s->t->paddingH);

				glTexCoord2f(s->tx / xw, s->ty / yw);
				glVertex2f(x, y);
				glTexCoord2f(s->tx / xw, (s->ty+s->h) / yw);
				glVertex2f(x, y+s->h);
				glTexCoord2f((s->tx+s->w) / xw, (s->ty+s->h) / yw);
				glVertex2f(x+s->w, y+s->h);
				glTexCoord2f((s->tx+s->w) / xw, s->ty / yw);
				glVertex2f(x+s->w, y);
			}
			if (tt)
				glEnd();
		}

		glDisable(eng->render->textureType);
		glDisable(GL_CULL_FACE);

		glEndList();
	}
}
#endif

void TileMng::FromFile(FILE *fp, int len, bool detectSize)
{
	Free();

	num = len/29;

#ifndef NDEBUG
	printf("Tiles: %d\n", num);
#endif

	//bool detectSize = true; // cam-lock via tiles
	if (eng->render->cam.maxX || eng->render->cam.maxY)
		detectSize = false;

	float maxHeight = 0;
	float maxWidth = 0;

	for (int i=0; i<num; i++) {
		Tile t;
		t.sub.t = TilesetID::GetTexture(fgetc(fp));

		fread(&t.sub.tx, sizeof(int), 1, fp);
		fread(&t.sub.ty, sizeof(int), 1, fp);
		fread(&t.sub.w, sizeof(int), 1, fp);
		fread(&t.sub.h, sizeof(int), 1, fp);
		
		int depth;
		fread(&depth, sizeof(int), 1, fp);
		if (depth >= -99)
			depth = 0;
		else if (depth >= -100)
			depth = 1;
		else
			depth = 2;

		int xx,yy;
		fread(&xx, sizeof(int), 1, fp);
		fread(&yy, sizeof(int), 1, fp);
		
		t.x = (float)xx;
		t.y = (float)yy;

		// add tile to vector
		tileV[depth].push_back(t);

		// map h
		unsigned int z = tileV[depth].size() - 1;
		if (tileV[depth][z].y+tileV[depth][z].sub.h > maxHeight)
			maxHeight = (float)tileV[depth][z].y+tileV[depth][z].sub.h;
	}
	if (detectSize) { // lock camera
		if (maxWidth)
			eng->render->cam.maxX = (float)maxWidth - 640.0f;
		if (maxHeight)
			eng->render->cam.maxY = (float)maxHeight - 480.0f;
	}

#ifdef TILEMNG_USE_DISPLAY_LISTS
	// sort arrays by texture ID
	// NOTE: THis messes up the order in which the tiles were placed in the editor, so I've disabled it
	/*
	for (int i=0; i<3; i++)
		std::sort(tileV[i].begin(), tileV[i].end(), SortByTexture);
	*/

	GenDisplayList();
#endif
}

void TileMng::Draw(int depth)
{
	Prof(TileMng_Draw);

	if (depth >= -99)
		depth = 0;
	else if (depth >= -100)
		depth = 1;
	else
		depth = 2;

#ifdef TILEMNG_USE_DISPLAY_LISTS
	if (tileV[depth].size())
		glCallList(tileDList[depth]);
#else
	for (unsigned int i=0; i<tileV[depth].size(); i++) {
		eng->render->DrawSubImage(&tileV[depth][i].sub, tileV[depth][i].x, tileV[depth][i].y);
	}
#endif
}
