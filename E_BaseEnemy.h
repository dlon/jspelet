#pragma once
#ifndef E_BASEENEMY_H
#define E_BASEENEMY_H

#include "Entity.h"
#include "E_Player.h"

class E_BaseEnemy : public Entity
{
protected:
	bool			active; // activate when player gets in range
	bool			manualActivation;
	float			health;
	int				hurtTimer;
	float			blendMp;
	float			playerDamage;
	E_Player	*	player;				// static E_Player *player
	float			activRadius;
	static sf::Sound	sndSpltr;
	int				lifeSpan;
public:
	E_BaseEnemy(float maxHealth);
	virtual ~E_BaseEnemy() {}

	virtual bool IsEnemy() { return true; }
	virtual float Life() { return health; }
	virtual void Draw();
	virtual void Step();
	virtual void PostStep();
	virtual void CheckTiles(SolidMap *solidMap);
	virtual void Hurt(float hp);
	virtual void Collision(Entity *with);
	//virtual void CallTimer(int num);
	virtual void PostCreate(); // !?!?

	virtual void IEditorSettings(int lifeSpan,
		int activeRadius,
		bool manualActivation,
		bool initiallyActive);
};

#endif // E_BASEENEMY_H
