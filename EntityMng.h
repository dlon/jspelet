#pragma once
#ifndef ENTITYMNG_H
#define ENTITYMNG_H

#include "Entity.h"
//#include "TileMng.h"
#include "SolidMap.h"
#include <list>
#include "Archiver.h"

class EntityMng;

struct TimerSpawner // class for delayed spawning
{
	struct einfo {
		int id; int frames;
		float x; float y;
		bool	versionFour;
		int		lifeSpan;
		int		activeRadius;
		bool	manualActivation;
		bool	initiallyActive;
		int		piggSpeed;
	};
	std::list< einfo > m;

	void SpawnDelayedEntities();

	TimerSpawner(EntityMng *entMng) : entMng(entMng) {}
private:
	EntityMng *entMng;
};

class EntityMng
{
	int prevUpdate;
	std::list<Entity*> ents;
	TimerSpawner timerSpawner;

	Entity *player;
	Entity *boss;
public:
	Entity *CreateEntity(const char *name);
	Entity *CreateEntity(int id);
	//Entity *AddEntity(Entity *e);

	void FromFile(FILE *fp, int len, bool versionFour, bool versionFive); // map
	void Serialize(Archiver &arc);
	bool Update(/*TileMng*/SolidMap *tiles); // returns whether it tick'd
	void Draw();
	void Free();

	void	SetBoss(Entity *boss) { this->boss = boss; }
	Entity *GetBoss() { return boss; }

	Entity *FindInstanceById(Entity *pos, int id=-1);
	Entity *FindClosestInstance(float x, float y, float maxDist, int id=-1); // actually looks for any instance within range
	//Entity *CollisionAtPos(Entity *me, float x, float y);

	void GenerateCollisions(Entity *e, SolidMap *tiles, std::list<Entity*>::iterator offset);

	EntityMng();
	~EntityMng();
};

#endif // ENTITYMNG_H
