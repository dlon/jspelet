#pragma once
#ifndef E_COPYCAT_H
#define E_COPYCAT_H

#include "E_BaseEnemy.h"
#include "SoundFactory.h"
#include "Sound.h"

class E_CopyCat : public E_BaseEnemy
{
	int		attack;
	float	moveBackX,moveBackY;
	bool	retreatQuick;
	float	px,py;
public:
	Sprite	spr;

	int GetID() { return EID_COPYCAT; }

	void	Step();
	void	PostStep();
	void	CallTimer(int num);
	void	CheckTiles(SolidMap *solidMap);
	void	Collision(Entity *with);

	E_CopyCat();
};

#endif // E_COPYCAT_H
