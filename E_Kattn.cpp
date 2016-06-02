#include "E_Kattn.h"

#define KATTN_SPEED 2.5f

/* Hoppkod
const int kJumpTimer = 30;
*/

E_Kattn::E_Kattn() : E_BaseEnemy(40.0f)
{
	mainSpr.Load("data/cp.PNG", 4, 46, 50);
	
	spriteIndex = &mainSpr;
	spriteIndex->imgSpeed = .15f;

	colRect.SetW(45.0f);
	colRect.SetH(49.0f);
	colRect.lSlopeOff = 18.0f;
	colRect.rSlopeOff = 18.0f;

	/* Hoppkod
	SetTimer(0, kJumpTimer);

	manualActivation = true;
	active = true;
	*/

	playerDamage = 15.0f;
}

/* Hoppkod
void E_Kattn::CallTimer(int num)
{
	E_BaseEnemy::CallTimer(num);
	if (!num)
		vspeed = -15.0f;
}
*/

void E_Kattn::Step()
{
	E_BaseEnemy::Step();

	/* Hoppkod
	// reset timer
	if (GetTimer(0)==-1 && vspeed==0.0f)
		SetTimer(0, kJumpTimer);
	*/

	if (player) {
		if (player->x - x < -40.0f)
			hspeed = -KATTN_SPEED;
		else if (player->x - x > 40.0f)
			hspeed = KATTN_SPEED;
		//else keep
	}
}

void E_Kattn::PostStep()
{
	if (hspeed < .0f)
		spriteIndex->NormalFlip();
	else if (hspeed > .0f)
		spriteIndex->MirrorFlip();

	/* Hoppkod
	if (vspeed != 0.0f)
		spriteIndex->imgSpeed = .15f;
	else {
		spriteIndex->imgInd = 0;
	}
	*/

	E_BaseEnemy::PostStep();
}
