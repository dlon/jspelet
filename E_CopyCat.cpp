#include "E_CopyCat.h"
#include "Engine.h"
#include "Jack.h"
#include <assert.h>

extern Engine *eng;

static int		kAttackTime = 180;
static int		kMoveAwayTime = 180;
static int		kMoveAwayQuicklyTime = 30;
static float	kSpeed = 4.5f;

E_CopyCat::E_CopyCat() : E_BaseEnemy(50.0f)
{
	spr.Load("data/Copycat.png", 4, 91, 0);
	
	spriteIndex = &spr;
	spriteIndex->imgSpeed = .2f;
	
	//colRect.FromSprite(&spr);
	//colRect.SetXOff(-45.0f); // offset
	//spriteIndex->xofs = -45.0f;
	colRect.SetXOff(-30.0f); // offset
	colRect.SetYOff(13.0f);
	colRect.SetW(51.0f);
	colRect.SetH(53.0f);
	spriteIndex->xofs = -45.0f;

	gravity = 0;
	activRadius = 1200.0f;

	attack = 0;
	SetTimer(0, kAttackTime);
	SetTimer(1, 1); // update player repr.

	moveBackX = moveBackY = 0;
	retreatQuick = false;
}

void E_CopyCat::Step()
{
	E_BaseEnemy::Step();

	if (!active)
		return;

	if (player) {

		if (attack==1) {
			//float distq = player->x*player->x + player->y+player->y;
			float distq = px*px + py*py;

			if (distq > 200.0f) {
				/*
				if (player->x - x < -10.0f)
					x -= kSpeed;
				if (player->x - x > 10.0f)
					x += kSpeed;
				if (player->y - y < -10.0f)
					y -= kSpeed;
				if (player->y - y > 10.0f)
					y += kSpeed;
				*/
				if (px - x < -10.0f)
					x -= kSpeed;
				if (px - x > 10.0f)
					x += kSpeed;
				if (py - y < -10.0f)
					y -= kSpeed;
				if (py - y > 10.0f)
					y += kSpeed;
			}
		}
		else if (attack==2) {
			// back away
			if (moveBackX-x > 10.0f)
				x += kSpeed;
			if (moveBackX-x < -10.0f)
				x -= kSpeed;
			if (moveBackY-y > 10.0f)
				y += kSpeed;
			if (moveBackY-y < -10.0f)
				y -= kSpeed;
		}
	}
}

void E_CopyCat::PostStep()
{
	// set face
	if (hspeed > 0.0f || prevx < x)
		spriteIndex->MirrorFlip();
	else if (hspeed < 0.0f || prevx > x)
		spriteIndex->NormalFlip();
	
	if (player && !hspeed && prevx == x) { // if still, face towards player
		if (player->x < x)
			spriteIndex->NormalFlip();
		else
			spriteIndex->MirrorFlip();
	}

	E_BaseEnemy::PostStep();
}

void E_CopyCat::CallTimer(int num)
{
	E_BaseEnemy::CallTimer(num);

	switch (num)
	{
	case 0:
		if (!attack || attack==2) {
			// attack
			retreatQuick = false;
			attack = 1;
		}
		else {
			// retreat
			float rDir = (float)(5+rand()%26)/10.0f;
			moveBackX = x + 200.0f * cos(rDir);
			moveBackY = y - 200.0f * sin(rDir);
			attack = 2;
		}

		if (attack==1)
			SetTimer(0, kMoveAwayTime); // TODO: increase timer length (chase time) but decrease it if the player is successfully hit
		else
			SetTimer(0, kAttackTime);
		break;

	case 1:
		assert(player);
		px = player->x;
		py = player->y;
		SetTimer(1, 40);
		break;
	}
}

void E_CopyCat::CheckTiles(SolidMap *solidMap)
{
	if (active) {
		colRect.SetY(y+vspeed);
		colRect.SetX(x+hspeed);
	}
}

void E_CopyCat::Collision(Entity *with)
{
	E_BaseEnemy::Collision(with);

	if (!active)
		return;

	if (!retreatQuick && with->GetID() == EID_PLAYER) {
		retreatQuick = true;
		
		//if (kMoveAwayQuicklyTime < GetTimer(0))
		SetTimer(0, kMoveAwayQuicklyTime);
		
		//puts("hai");
		//printf("%d\n", GetTimer(0));
	}
}
