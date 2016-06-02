#pragma once

#include "Entity.h"

class E_EndMap : public Entity
{
public:
	E_EndMap();

	Sprite	spr;

	int		GetID()						{ return EID_ENDMAP; }
	void	Collision(Entity *with);
};
