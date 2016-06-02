#pragma once
#ifndef E_BASECONTROLLED_H
#define E_BASECONTROLLED_H

#include "Entity.h"

class E_BaseControlled : public Entity
{
	int dir;
	int air;
public:
	E_BaseControlled() : dir(0), air(0) {}

	virtual void Step();
protected:
	virtual void MoveLeft() = 0;
	virtual void MoveRight() = 0;
	virtual void Jump() = 0;
	virtual void Idle() = 0;
};

#endif // E_BASECONTROLLED_H
