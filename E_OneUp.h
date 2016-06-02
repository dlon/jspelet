#pragma once
#include "Entity.h"
#include "Sound.h"

class E_OneUp : public Entity
{
public:
	Sprite	sprite;
	Sound	*snd;

	int		GetID() { return EID_ONEUP; }

	E_OneUp();
	void	Collision(Entity *with);
};
