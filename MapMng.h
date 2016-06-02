#pragma once
#ifndef MAPMNG_H
#define MAPMNG_H

#include "EntityMng.h"
#include "TileMng.h"
#include "SolidMap.h"
#include "Archiver.h"
#include <string>
#include "GeomMng.h"
#include "particles/particles.h"

enum cpt_e {
	CPT_HORI = 1,
	CPT_VERT = 2
};

struct CheckPointInfo
{
	int		id;
	float	x,y;
	int		dir;
	bool	taken;
};

class MapMng
{
	TileMng		tiles;
	EntityMng	entities;
	SolidMap	solids;
	std::string	mapFile;
	GeomMng		geom;

	std::vector<CheckPointInfo> checkpoints;
public:
	MapMng();
	~MapMng() { Free(); }

	ParticleManager		partMng;

	void Load(const char *file, bool loadTiles=true, bool loadSolids=true, bool loadEntities=true, bool loadGeometry=true); // TODO: int loadFlag
	void Reload();
	void Free();

	bool Update();
	void Draw();

	void Serialize(Archiver &arc);

	const char *MapFile() { return mapFile.c_str(); }

	EntityMng &		GetEntities() { return entities; }
	TileMng &		GetTiles() { return tiles; }
	SolidMap &		GetSolids() { return solids; }

	CheckPointInfo	*CheckPointAt(float x, float y, float w, float h);
	CheckPointInfo	*CheckPointId(int id);
};

#endif // MAPMNG_H
