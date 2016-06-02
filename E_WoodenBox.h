#pragma once

#include "Entity.h"

enum WoodenType {
	WOOD_NORMAL,
	WOOD_RESPAWN
};

class E_WoodenBox : public Entity
{
	bool		colliding;
	float		friction;
	float		mHspeed; // works only for one object at a time!
	Entity *	mOther;
	int			id;
	float		orgX,orgY;
public:
	bool		onPlatform;
	float		platformH;
	float		platformV;

	E_WoodenBox(WoodenType type);

	int GetID() { return id; }
	//void Step();

	void Step();
	void Collision(Entity *with);
	void PostStep();
	bool IsPushable() { return true; }
	bool IsSolid() { return true; }

	float	Friction()		{ return friction; }

	Sprite sprite;
};
