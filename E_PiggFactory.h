#pragma once
#include "Entity.h"

class E_PiggFactory : public Entity
{
public:
	int		mPiggSpawnInterval;
	int		mPiggLifespan;
	float	mPiggSpeed;

	E_PiggFactory();

	int GetID() { return EID_PIGGFACTORY; }
	
	void		CallTimer(int num);
	void		ResetTimer();

	void IEditorSettings(int lifeSpan,
		int activeRadius,
		bool manualActivation,
		bool initiallyActive);
};
