#pragma once
#ifndef E_KITTY_H
#define E_KITTY_H

#include "E_BaseEnemy.h"
#include "SoundFactory.h"

class E_Kitty : public E_BaseEnemy
{
	bool foundTarget;
public:
	Sprite	mainSpr;
	Sprite	jumpSpr;
	sf::Sound	meowSnd;

	int GetID() { return EID_KITTY; }

	void Step();
	void PostStep();

	E_Kitty();
};

#endif // E_KITTY_H
