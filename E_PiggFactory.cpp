#include "E_PiggFactory.h"
#include "Jack.h"
#include "Engine.h"
#include "E_PiggAttack.h"

extern Engine *eng;

E_PiggFactory::E_PiggFactory()
{
	mPiggSpawnInterval = 120;
	mPiggLifespan = 120;
	mPiggSpeed = 16.0f;
	SetTimer(0, 1);
}

void E_PiggFactory::IEditorSettings(int lifeSpan,
		int activeRadius,
		bool manualActivation,
		bool initiallyActive)
{
	mPiggLifespan = lifeSpan;
}

void E_PiggFactory::ResetTimer()
{
	SetTimer(0, mPiggSpawnInterval);
}

void E_PiggFactory::CallTimer(int num)
{
	Entity *e = eng->jack->map.GetEntities().CreateEntity(EID_PIGGATTACK);
	if (e) {
		e->x = x;
		e->y = y;
		static_cast<E_PiggAttack*>(e)->SetLifespan(mPiggLifespan);
		static_cast<E_PiggAttack*>(e)->SetSpeed(mPiggSpeed);
	}
	SetTimer(0, mPiggSpawnInterval);
}
