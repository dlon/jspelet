#pragma once
#include "Entity.h"

class E_OneUp : public Entity
{
public:
	Sprite	sprite;
	sf::Sound	snd;

	int		GetID() { return EID_ONEUP; }

	E_OneUp();
	void	Collision(Entity *with);
};
