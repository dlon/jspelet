#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include "CollisionObject.h"

class Object
{
public:
	virtual CollisionBox *GetCollisionBox() { return 0; }

	virtual ~Object() {}
};

#endif // OBJECT_H
