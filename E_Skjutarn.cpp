#include "E_Skjutarn.h"
#include "Engine.h"
#include "E_LazerBeam.h"
#include "Jack.h"

#define SHOOT_DELAY 3000

extern Engine *eng;

E_Skjutarn::E_Skjutarn() : E_BaseEnemy(40.0f)
{
	mainSpr.Load("data/skjutarn.png", 1, 0,0);
	aLazer = Sound(*eng->res->sounds.Load("data/zap.wav"));
	
	spriteIndex = &mainSpr;

	colRect.SetW((float)mainSpr.GetWidth());
	colRect.SetH((float)mainSpr.GetHeight()-1.0f);

	shootAt = eng->time;
	shAllow = false;

	//activRadius = 400.0f;
}

void E_Skjutarn::Step()
{
	E_BaseEnemy::Step();

	if (!active)
		return;

	if (//!shAllow &&
		x >= eng->render->cam.x + 50.0f && x < eng->render->cam.x + 590.0f // within camera range
		)
	{
		shAllow = true;
	}
	else
		shAllow = false;

	if (player) {
		if (player->x < x)
			spriteIndex->NormalFlip();
		else
			spriteIndex->MirrorFlip();
	}

	if (shAllow && shootAt < eng->time) {
		shootAt = eng->time + SHOOT_DELAY;

		// beam
		Entity *lazer = eng->jack->map.GetEntities().CreateEntity(EID_LAZERBEAM); // add(new E_LazerBeam)
		lazer->x = (!spriteIndex->Flipped() ? x-20.0f : x+20.0f);
		lazer->y = y+40.0f;
		lazer->hspeed = (!spriteIndex->Flipped() ? -20.0f : 20.0f);
		static_cast<E_LazerBeam*>(lazer)->SetEnemy();
		static_cast<E_LazerBeam*>(lazer)->SetColor(0,255,0);
		static_cast<E_LazerBeam*>(lazer)->ShortenLife(6);

		//aLazer->PlayLoc(x - player->x, y - player->y, true);
		aLazer.play(); // FIXME-SFML: pan
	}
}
