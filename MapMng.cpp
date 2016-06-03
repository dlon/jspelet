#include "MapMng.h"
#include <stdio.h>
#include <assert.h>
#include "Engine.h"
#include "Jack.h"

extern Engine *eng;

extern bool		plResetChkPt;
extern int		plCheckPointId;

// particles
static EmitterProperties rocketP = {
	0.0f, 0.0f, // distribution
	0.9f, 0.9f, // velocity
	90.0f, 180.0f, // direction
	1.0f, 0.2f, 0.001f, 1.0f, // color
	1.0f, 0.2f, 0.001f, 1.0f,
	2.0f, 2.5f, // lifespan
	0.1f, 0.4f, // scale
	false, // tail
	50, // rate
	1,
	false
};
static EmitterProperties rSmokeProp = {
	0.0f, 0.0f, // distribution
	1.0f, 1.0f, // velocity
	90.0f, 180.0f, // direction
	0.2f, 0.2f, 0.2f, 0.1f, // color
	0.2f, 0.2f, 0.2f, 0.1f,
	3.0f, 3.0f, // lifespan
	0.5f, 0.5f, // scale
	false, // tail
	30, // rate
	0,
	false
};

//
// map mng
//
enum
{
	SOLID_TAGV1,
	TILE_TAGV1,
	ENTITY_TAGV1,
	TILE_TAGV2,
	ENTITY_TAGV2,

	SONG_TAGV1		= 500,
	EDITOR_TAGV1	= 600,	// none of our business
	SIZE_TAGV1		= 700,
	GEOM_TAGV1		= 800,
	
	ENTITY_TAGV3	= 900,
	ENTITY_TAGV4,
	ENTITY_TAGV5,
	SKYCOL_TAGV1	= 1000,
	CHECKPOINT_TAGV1 = 1100
};

static const char *songList[] = {
	NULL,
	"data/xgx.ogg",
	"data/zSnes3.ogg",
	"data/winlyfe.ogg",
	NULL,
	"data/boss1.ogg",
	"data/asaszszx2.ogg",
	"data/hryp.ogg",
	"data/jkajkfa.ogg",
	"data/nsns.ogg",
	"data/ccccc.ogg",
	"data/xc.ogg",
	"data/snes2.ogg",
	"data/pltf.ogg",
	"data/shjak.ogg"
};

static
const char *IdentifySong(int i) {
	if (i >= 0 && i < sizeof(songList)/sizeof(songList[0]))
		return songList[i];
	return 0;
}

static void FixPath(const char *file, std::string *ret)
{
	if (*file == '\0')
		return;

	// find relative path the lazy way
	const char *p;
	while (p = strstr(file+1, "data/"))
		file = p;
	while (p = strstr(file+1, "data\\"))
		file = p;
	
	// replace \\ with /
	ret->assign(file);
	for (size_t i=0; i<strlen(file); i++) {
		if ((*ret)[i] == '\\')
			(*ret)[i] = '/';
	}
}

MapMng::MapMng() : solids(*this)
{
	eng->res->textures.Load("data/fire.tga"); // FIXME-SFML: Why are these loaded here?
	eng->res->textures.Load("data/smoke.tga");
}

void MapMng::Reload()
{
	Load(mapFile.c_str());
}

void MapMng::Free()
{
	checkpoints.clear();
}

void MapMng::Load(const char *file, bool loadTiles, bool loadSolids, bool loadEntities, bool loadGeometry)
{
	FILE *fp = fopen(file, "rb");
	if (!fp)
		return; // throw;

	eng->render->cam.maxX = 0; // cam lock
	eng->render->cam.maxY = 0;
	bool detectSize = true;

	unsigned char col_r = 148;
	unsigned char col_g = 200;
	unsigned char col_b = 225;

	// clear map
	// todo: clear the other stuff? just in case there are no tags.
	geom.Clear();

	// FIXME: preliminary
	tiles.Free();
	solids.Clear();
	entities.Free();
	//

	checkpoints.clear();
	if (plResetChkPt)
		plCheckPointId = -1; // E_Player

	do
	{
		int len;
		int idx;
		fread(&len, sizeof(int), 1, fp);
		fread(&idx, sizeof(int), 1, fp);
		
		if (feof(fp))
			break;

		switch(idx)
		{
		case CHECKPOINT_TAGV1:
			int cx,cy;
			unsigned int t;

			for (int i=0; i<(len-4)/8; i++)
			{
				fread(&cx,sizeof(cx),1,fp);
				fread(&cy,sizeof(cy),1,fp);
				t = fgetc(fp);

				CheckPointInfo cpi;
				switch (t)
				{
				case 0: cpi.dir = CPT_HORI; break; // actually a vertical *line*
				case 1: cpi.dir = CPT_VERT; break;
				case 2: cpi.dir = CPT_HORI|CPT_VERT; break;
				}
				cpi.x = (float)cx;
				cpi.y = (float)cy;
				cpi.id = checkpoints.size();
				cpi.taken = false;

				checkpoints.push_back(cpi);
			}
			break;
		case SIZE_TAGV1:
			int mX,mY;
			fread(&mX,sizeof(mX),1,fp);
			fread(&mY,sizeof(mY),1,fp);

			if (mX == -2 || mY == -2)
				detectSize = false; // no size
			else {
				if (mX != -1)
					eng->render->cam.maxX = (float)mX-640;
				if (mY != -1)
					eng->render->cam.maxY = (float)mY-480;
			}
			printf("Size tag: %d x %d\n", mX,mY);
			break;
		case SOLID_TAGV1:
			if (loadSolids)
				solids.FromFile(fp, len-4);
			else
				fseek(fp, len-4, SEEK_CUR);
			break;
		case TILE_TAGV2:
			if (loadTiles)
				tiles.FromFile(fp, len-4, detectSize);
			else
				fseek(fp, len-4, SEEK_CUR);
			break;
		case ENTITY_TAGV2:
			//MessageBox(0, "Old entity tag", 0, 0);
			break;
		case ENTITY_TAGV3:
			if (loadEntities)
				entities.FromFile(fp, len-4, false, false);
			else
				fseek(fp, len-4, SEEK_CUR);
			break;
		case ENTITY_TAGV4:
			if (loadEntities)
				entities.FromFile(fp, len-4, true, false);
			else
				fseek(fp, len-4, SEEK_CUR);
			break;
		case ENTITY_TAGV5:
			if (loadEntities)
				entities.FromFile(fp, len-4, true, true);
			else
				fseek(fp, len-4, SEEK_CUR);
			break;
		case SONG_TAGV1:
			{
				int i;
				fread(&i, sizeof(i), 1, fp);
				const char *song = IdentifySong(i);

				if (!song)
					printf("Undefined song id '%d'.\n", i);
				if (song)
					eng->jack->SetSong(song);
			}
			break;
		case GEOM_TAGV1:
			if (loadGeometry)
				geom.FromFile(fp, len-4);
			else
				fseek(fp, len-4, SEEK_CUR);
			break;
		case SKYCOL_TAGV1:
			fread(&col_r, sizeof(col_r), 1, fp);
			fread(&col_g, sizeof(col_g), 1, fp);
			fread(&col_b, sizeof(col_b), 1, fp);
			break;
		default:
			printf("Unidentified tag '%d'.\n", idx);
			fseek(fp, len-4, SEEK_CUR);
		}
	} while (!feof(fp));
	fclose(fp);

	printf("Cam lock: %fx%f\n", eng->render->cam.maxX, eng->render->cam.maxY);

	//mapFile.assign(file);
	FixPath(file, &mapFile); // store path
	eng->render->SetClearColor(col_r, col_g, col_b);
}

bool MapMng::Update()
{
	// test
	static int pp = 0;
	static int prev = 0;
	if (!prev)
		prev = eng->time;
	pp += eng->time - prev;

	if (pp >= 16) {
		if (pp > 500) pp = 16; // freeze fix
			
		for (; pp >= 16; pp-=16)
		{
			/*
			ParticleEmitter *pe;
			if ((pe = partMng.GetEmitter("rocketSmoke")))
				pe->Emit();
			if ((pe = partMng.GetEmitter("rocket")))
				pe->Emit();
				*/
			partMng.Update();
		}
	}
	prev = eng->time;
	
	//
	return entities.Update(&solids);
}

void MapMng::Draw()
{
	geom.Draw(GEOM_LAYER_BG);
	tiles.Draw(-99);
	geom.Draw(GEOM_LAYER_NORMAL);
	tiles.Draw(-100);
	entities.Draw();
	geom.Draw(GEOM_LAYER_FG);
	tiles.Draw(-102);

	bool		blend;
	RBLENDMODE	rbm;
	eng->render->GetBlendMode(&rbm, &blend);
	partMng.Draw();
	eng->render->SetBlendMode(rbm, blend);
}

void MapMng::Serialize(Archiver &arc)
{
	if (arc.Writable())
		arc << mapFile.c_str(); // map file name
	else
	{
		// load map
		std::string tmp;
		arc >> tmp;
		Load(tmp.c_str(), true, true, false);
	}
	entities.Serialize(arc); // load/save entity placements
}

CheckPointInfo *MapMng::CheckPointAt(float x, float y, float w, float h)
{
	for (unsigned i=0;
		i != checkpoints.size(); i++)
	{
		/*
		if (checkpoints[i].x >= x &&
			checkpoints[i].y >= y &&
			checkpoints[i].x < x+w &&
			checkpoints[i].y < y+h)
		{
			return &checkpoints[i];
		}
		*/
		if (checkpoints[i].taken)
			continue;
		checkpoints[i].taken = true;
		switch (checkpoints[i].dir)
		{
		case CPT_HORI:
			if (x > checkpoints[i].x)
				return &checkpoints[i];
			break;
		case CPT_VERT:
			if (y > checkpoints[i].y)
				return &checkpoints[i];
			break;
		case CPT_HORI|CPT_VERT:
			if (x > checkpoints[i].x && y > checkpoints[i].y)
				return &checkpoints[i];
			break;
		}
		checkpoints[i].taken = false;
	}
	return NULL;
}

CheckPointInfo *MapMng::CheckPointId(int id)
{
	/*
	for (unsigned i=0;
		i != checkpoints.size(); i++)
	{
		if (checkpoints[i].id == id)
		{
			return &checkpoints[i];
		}
	}
	return NULL;
	*/
	if (id >= checkpoints.size())
		return 0;
	return &checkpoints[id];
}
