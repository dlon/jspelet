#pragma once

//#include "E_BaseEnemy.h"
#include "E_BaseBoss.h"
#include "JackHUD.h"
#include "E_Player.h"

class E_BossTrigger : public Entity
{
	E_Player	*player;
public:
	E_BossTrigger();

	int		GetID() { return EID_BOSSTRIGGER; }
	void	Step();
};

class E_Boss : public E_BaseBoss, public BossInfo /* public E_BaseEnemy */
{
	float	maxHealth;
	bool	shooting;
	bool	attack2;
	bool	attack2Swift;
	bool	attack2Swift2;
	int		dropN;
	float	osc;
	bool	oscS;
	bool	oscB;
public:
	E_Boss();
	~E_Boss();

	Sprite spr;
	Sprite sprShoot;
	Sound *sndStep;

	int		GetID() { return EID_BOSSOBJECT; }
	void	Step();
	void	PostStep();

	void	DrawHealthBar(); // 

	float	GetMaxHealth()		{ return maxHealth; }
	float	GetHealth()			{ return health; }
	void	SetHealth(float hp)	{ health = hp; }

	void	CallTimer(int num);

	void	Attack();
};

class E_BossRocket : public Entity
{
public:
	E_BossRocket();

	Entity	*pl;
	Sprite	sprRocket;
	Sound	*sndRocket;
	Sound	*sndShoot;

	bool	IsEnemy()	{ return true; }
	int		GetID()		{ return EID_BOSSROCKET; }

	void	CheckTiles(SolidMap *sldMap);
	void	Collision(Entity *with);

	void	CallTimer(int num);
};

class E_BossSurprise : public Entity
{
public:
	E_BossSurprise();

	Entity	*pl;
	Sprite	sprSurprise;
	Sound	*sndDrop;
	Sound	*sndExplode;

	bool	firstExec;

	bool	IsEnemy()	{ return true; }
	int		GetID()		{ return EID_BOSSSURPRISE; }

	void	Step();
	void	Collision(Entity *with);

	void	CallTimer(int num);
};
