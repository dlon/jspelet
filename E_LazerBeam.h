#pragma once
#ifndef E_LAZER_H
#define E_LAZER_H

#include "Entity.h"
#include <assert.h>

class E_LazerBeam : public Entity
{
	bool enemy;
	int r,g,b;
public:
	E_LazerBeam(int dir=0, bool enemyBeam=false);

	void SetFriendly() { enemy = false; }
	void SetEnemy() { enemy = true; }
	void SetColor(int r,int g,int b) { assert(r<=255); assert(g<=255); assert(b<=255); this->r=r; this->g=g; this->b=b; }
	void ShortenLife(int amount);

	bool IsEnemy() { return enemy; }

	int GetID() { return EID_LAZERBEAM; }
	void Draw();
	void Step();
	void Collision(Entity *with); // destroy (esp solid)

	void CallTimer(int num);
};

#endif // E_LAZER_H
