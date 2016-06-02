#include "GeomMng.h"
#include "Engine.h"

#include "prof.h"

extern Engine *eng;

void GeomMng::Draw(GeomLayer depth) {
	Prof(GeomMng_Draw);

	std::vector<QuadInfo>::iterator it = quads[depth].begin();
	for (; it != quads[depth].end(); ++it)
	{
		eng->render->SetColor(it->r, it->g, it->b, it->a);
		eng->render->DrawRect(it->x, it->y, it->w, it->h);
	}
}

void GeomMng::Clear()
{
	quads[0].clear();
	quads[1].clear();
	quads[2].clear();
}

void GeomMng::FromFile(FILE *fp, int len)
{
	Clear();

	for (int i=0; i<len; ) {
		int vt,ext;
		fread(&vt, sizeof(int), 1, fp);	// vertices
		fread(&ext, sizeof(int), 1, fp);	// extra bytes

		i += 8 + vt*4 + ext;

		switch (vt)
		{
		case 4: // quad
			{
			QuadInfo	quad={0};
			quad.a = 255; // default opaque
			GeomLayer	dp = GEOM_LAYER_NORMAL;

			int x1,y1,x2,y2;
			fread(&x1, sizeof(int), 1, fp);
			fread(&y1, sizeof(int), 1, fp);
			fread(&x2, sizeof(int), 1, fp);
			fread(&y2, sizeof(int), 1, fp);

			// set dimensions
			if (x1 > x2) {
				quad.x = (float)x2;
				quad.w = (float)(x1-x2);
			}
			else {
				quad.x = (float)x1;
				quad.w = (float)(x2-x1);
			}
			if (y1 > y2) {
				quad.y = (float)y2;
				quad.h = (float)(y1-y2);
			}
			else {
				quad.y = (float)y1;
				quad.h = (float)(y2-y1);
			}

			// ext info
			if (ext >= 3) { // color
				quad.r = fgetc(fp);
				quad.g = fgetc(fp);
				quad.b = fgetc(fp);
			}
			if (ext >= 7) { // depth
				int depth;
				fread(&depth, sizeof(int), 1, fp);
				switch (depth)
				{
				case -100:
					dp = GEOM_LAYER_NORMAL;
					break;
				case -102:
					dp = GEOM_LAYER_FG;
					break;
				case -99:
					dp = GEOM_LAYER_BG;
					break;
				}
			}
			if (ext >= 8)
				quad.a = fgetc(fp); // alpha
			quads[dp].push_back(quad);
			}
			break;
		default: // skip
			fseek(fp, vt*4 + ext, SEEK_CUR);
			break;
		}
	}
}
