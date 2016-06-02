#pragma once
#ifndef E_PIGGATTACK_H
#define E_PIGGATTACK_H

#include "E_BaseEnemy.h"
#include "SoundFactory.h"
#include "Sound.h"

class E_PiggAttack : public E_BaseEnemy
{
	float speed;
public:
	Sprite mainSpr;
	int GetID() { return EID_PIGGATTACK; }
	
	void Step();
	void PostStep();
	void Collision(Entity *with);
	void CallTimer(int num);

	void SetLifespan(int life) { lifeSpan = life; }
	void SetSpeed(float speed) { this->speed = speed; }

	E_PiggAttack();
};

#endif // E_PIGGATTACK_H
