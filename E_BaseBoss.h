#pragma once
#include "E_BaseEnemy.h"

class E_BaseBoss : public E_BaseEnemy
{
	// todo: lock-cam options?
public:
	E_BaseBoss() : E_BaseEnemy(100.0f) {
		manualActivation = true;
	}
	virtual ~E_BaseBoss() {}

	//virtual void PostStep();

	void Activate(E_Player *player) { this->player = player; active = true; }
	void Deactivate() { player = NULL; active = false; }
};
