#pragma once

#include "E_BaseBoss.h"
#include "JackHUD.h"
#include "E_Player.h"
#include "Sound.h"

class E_Boss2Trigger : public Entity
{
	E_Player	*player;
	Sound		*sndShake;
	bool		trigger1;
	bool		shake;
public:
	E_Boss2Trigger();

	int		GetID() { return EID_BOSS2TRIGGER; }
	void	Step();
	void	CallTimer(int num);
};

class E_Boss2;

class E_Boss2Sequencer : public Entity
{
	E_Boss2 *boss;
public:
	E_Boss2Sequencer();

	void	SetEntity(E_Boss2 *e);
	int		GetID() { return EID_BOSS2SEQUENCER; }
	void	CallTimer(int num);

};

class E_Boss2 : public E_BaseBoss, public BossInfo /* public E_BaseEnemy */
{
	float	maxHealth;
	int		attack1;
	int		attack2;
	int		attack3,attack3_1;
public:
	int		stage;

	E_Boss2();
	~E_Boss2();

	Sprite	spr1;
	Sprite	spr2;
	Sound	*sndShake;

	E_Boss2Sequencer *seq;

	int		GetID() { return EID_BOSS2OBJECT; }
	void	Step();
	void	PostStep();

	float	GetMaxHealth()		{ return maxHealth; }
	float	GetHealth()			{ return health; }
	void	SetHealth(float hp)	{ health = hp; }

	void	CallTimer(int num);
};

class E_Boss2Fireball : public Entity
{
public:
	E_Boss2Fireball();

	Entity	*pl;
	Sprite	sprFireball;
	Sound*	snd1;

	bool	IsEnemy()	{ return true; }
	int		GetID()		{ return EID_BOSS2FIREBALL; }

	//void	CheckTiles(SolidMap *sldMap);
	void	Collision(Entity *with);

	void	CallTimer(int num);
};

class E_Boss2GrndFire : public Entity
{
	int		frHeight[1];
public:
	float	frAlpha[2];
	int		frBlend[1];

	E_Boss2GrndFire();

	Sprite	spr1;
	Sound	*snd1;

	bool	IsEnemy()	{ return true; }
	int		GetID()		{ return EID_BOSS2GRNDFIRE; }

	void	CallTimer(int num);

	void	Collision(Entity *with);
	void	PostStep();
	void	Draw();
};

class E_Boss2Shield : public Entity
{
	float	a;
	int		fMode;
	bool	solid;
public:
	Sprite	spr1;
	Sound*	snd1;
	Sound*	snd2;

	E_Boss2Shield();

	bool	IsEnemy()	{ return true; }
	bool	IsSolid()	{ return solid; }
	int		GetID()		{ return EID_BOSS2SHIELD; }
	void	CallTimer(int num);
	void	Step();
	void	Collision(Entity *with);
	void	PostStep();
	void	Draw();
};

class E_Boss2Door : public Entity
{
public:
	Sound*		snd;
	Sound*		sndOpen;
	int			wallN;
	E_Player *	player;
	float		wallHeight;
	Texture		*tset;

	E_Boss2Door();
	~E_Boss2Door();

	void	CreateWall();
	void	DestroyWall();

	int		GetID() { return EID_BOSS2DOOR; }

	void	CallTimer(int num);
	bool	IsSolid()	{ return true; }

	void	Draw();
};
