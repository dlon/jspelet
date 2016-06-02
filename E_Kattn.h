#pragma once
#ifndef E_KATTN_H
#define E_KATTN_H

#include "E_BaseEnemy.h"

class E_Kattn : public E_BaseEnemy
{
public:
	Sprite mainSpr;
	int GetID() { return EID_KATTN; }
	
	void Step();
	void PostStep();

	/* Hoppkod
	void CallTimer(int num);
	*/

	E_Kattn();
};

#endif // E_KATTN_H
