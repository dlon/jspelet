#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include "Sprite.h"
#include "CollisionObject.h"
#include "Archiver.h"
#include "EntityID.h"
#include <SFML/Audio.hpp>

#define GRAVITY_LIMIT 10.0f
#define MAX_TIMERS 13

typedef short int etimer_t;

class TileMng;
class SolidMap;

class Entity
{
	etimer_t	timers[MAX_TIMERS];
	int			waterBlockTimer;
public:
	float x,y;
	float prevx,prevy;
	float gravity;
	float hspeed,vspeed;
	float prevHspeed,prevVspeed;
	bool destroy;
	Sprite *spriteIndex;
	CollisionObject colRect;
	bool fallThroughTile; // fall through tile (if penetrable)
	bool inWater;
	//bool pullOutOfWater;

	virtual void PostCreate() {}

	virtual int GetID() = 0; // EntityID.h
	virtual bool IsPlayer() { return false; }
	virtual bool IsItem() { return false; }
	virtual bool IsEnemy() { return false; }
	virtual bool IsPushable() { return false; }
	virtual bool IsSolid() { return false; }
	virtual bool IsMovingPlatform() { return false; }

	virtual void Step();
	virtual void CheckTiles(/*TileMng*/SolidMap *tileMng);
	virtual void Collision(Entity *with);
	virtual void PostStep();
	virtual void Draw();

	virtual void Hurt(float hp) {}
	virtual float Life() { return 0.0f; }

	virtual void Serialize(Archiver &arc);

	virtual void	CallTimer(int num) {}
	void			SetTimer(int num, int frames);
	int				GetTimer(int num);

	virtual void	IEditorSettings(int lifeSpan,
		int activeRadius,
		bool manualActivation,
		bool initiallyActive) {
	}

	Entity();
	virtual ~Entity() {}
};

#endif // ENTITY_H
