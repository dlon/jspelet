#pragma once
#include "E_BaseEnemy.h"

class E_CatFish : public E_BaseEnemy
{
public:
	E_CatFish();

	int		GetID() { return EID_CATFISH; }
	void	CallTimer(int num);

	Sprite spr;
};
