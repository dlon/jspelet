#pragma once
#ifndef E_SKJUTARN_H
#define E_SKJUTARN_H

#include "E_BaseEnemy.h"

class E_Skjutarn : public E_BaseEnemy
{
	int		shootAt;
	bool	shAllow;
public:
	Sprite	mainSpr;
	Sound	*aLazer;

	int		GetID() { return EID_SKJUTARN; }
	void	Step();

	E_Skjutarn();
};

#endif // E_SKJUTARN_H
