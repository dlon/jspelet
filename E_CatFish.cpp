// FIXME: Irregular movement. It moves too far left sometimes. Timing issue? Will waiting until object is activated help? [I think it's fixed now]
//					idea: the timer is set before the object may be activated. it begins moving on irregular intervals, thus fucks up

#include "E_CatFish.h"
#include "Engine.h"

#define PATROL 100

extern Engine *eng;

E_CatFish::E_CatFish() : E_BaseEnemy(150.0f) {
	spr.Load("data/kfisk.png", 4, 140, 50);
	spr.imgSpeed = -0.2f;
	spriteIndex = &spr;

	colRect.SetW(140.0f);
	colRect.SetH(50.0f);

	gravity = 0;

	hspeed = -2.5f;
	SetTimer(0, PATROL);
}

void E_CatFish::CallTimer(int num)
{
	E_BaseEnemy::CallTimer(num);

	switch (num) {
		case 0:
			// flip dir
			hspeed *= -1.0f;
			spriteIndex->Flip();
			SetTimer(0, PATROL);
			break;
	}
}
