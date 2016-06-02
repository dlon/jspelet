#include "E_PiggAttack.h"

#define KATTN_SPEED 3.0f

const int kJumpTimer = 30;

E_PiggAttack::E_PiggAttack() : E_BaseEnemy(1000.0f)
{
	mainSpr.Load("data/PIGGATTACK3K5.PNG", 1, 0,0);
	
	spriteIndex = &mainSpr;
	colRect.FromSprite(&mainSpr);

	SetTimer(0, kJumpTimer);

	manualActivation = true;
	active = true;

	playerDamage = 20.0f;

	speed = 16.0f;
}

void E_PiggAttack::CallTimer(int num)
{
	E_BaseEnemy::CallTimer(num);
	if (!num)
		vspeed = -speed;
}

void E_PiggAttack::Step()
{
	E_BaseEnemy::Step();

	// reset timer
	if (GetTimer(0)==-1 && vspeed==0.0f)
		SetTimer(0, kJumpTimer);

	/*
	if (player) {
		if (player->x < x)
			hspeed = -KATTN_SPEED;
		else
			hspeed = KATTN_SPEED;
	}
	*/
}

void E_PiggAttack::PostStep()
{
	E_BaseEnemy::PostStep();
}

void E_PiggAttack::Collision(Entity *with)
{
	Entity::Collision(with);
	//E_BaseEnemy::Collision(with);

	if (with->GetID() == EID_PLAYER) {
		with->Hurt(playerDamage);
	}
}
