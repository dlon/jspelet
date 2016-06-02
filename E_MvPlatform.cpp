#include "E_MvPlatform.h"
#include "Jack.h"
#include "Engine.h"

extern Engine *eng;

#define MOVMNT_SPEED	1.5f

E_MvPlatform::E_MvPlatform(PLATFORM_DIR dir)
{
	spr.Load("data/plattform.png", 1, 0,0);
	colRect.FromSprite(&spr);

	spriteIndex = &spr;

	mTimerDelay = 240;
	gravity = 0;
	SetDir(dir);
}

void E_MvPlatform::SetDir(PLATFORM_DIR dir)
{
	this->dir = dir;

	hspeed = vspeed = 0.0f;

	float r = 0.0f; //float(rand() % 11)/10.0f - 0.5f;

	switch (dir)
	{
	case PLDIR_UP:
		vspeed = -MOVMNT_SPEED + r;
		break;
	case PLDIR_DOWN:
		vspeed = MOVMNT_SPEED + r;
		break;
	case PLDIR_LEFT:
		hspeed = -MOVMNT_SPEED + r;
		break;
	case PLDIR_RIGHT:
		hspeed = MOVMNT_SPEED + r;
		break;
	}

	SetTimer(0, mTimerDelay);
}

void E_MvPlatform::ResetTimer()
{
	SetTimer(0, mTimerDelay);
}

void E_MvPlatform::CallTimer(int num)
{
	if (num == 0) {
		vspeed *= -1.0f;
		hspeed *= -1.0f;
		SetTimer(0, mTimerDelay);
	}
	else if (num == 1) {
		if (eng->jack->map.GetEntities().FindClosestInstance(x + colRect.GetW()/2, y + colRect.GetH()/2, 100.0f, EID_PLAYER))
			SetDir(dir);
		else
			SetTimer(1, 30);
	}
}

void E_MvPlatform::CheckTiles(SolidMap *tileMng)
{
	colRect.SetY(y+vspeed);
	colRect.SetX(x+hspeed);
}

void E_MvPlatform::IEditorSettings(int lifeSpan,
		int activeRadius,
		bool manualActivation,
		bool initiallyActive)
{
	if (manualActivation) {
		// activate on player col
		SetTimer(0, 0);
		hspeed = vspeed = 0;
		SetTimer(1, 30);
	}
}
