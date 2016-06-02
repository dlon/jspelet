#pragma once
#ifndef E_PLAYER_H
#define E_PLAYER_H

#include "Entity.h"
#include "JackHUD.h"

class E_Player : public Entity, public JackPlayerInfo
{
	int		jumpFuelPoint;
	bool	sprint;
	bool	jump;
	float	health;
	bool	hasPlayedSplash;
	int		swimFuel;
	int		blasterTimer;
	int		doubleJumpTimer;
	int		pullThroughWaterTimer;
	int		healthTimer;
	bool	lockInput;
	int		dying;
	int		emotion;
	float	spritesAlpha[3];
	float	jHspeed; // damn you, relativity!
	bool	onPlatform;
	float	platformHs;
	int		hurt;
public:
	E_Player();

	Sprite sPlayer[3];
	Sprite sPlayerJmp[3];
	Sprite sPlayerDead;
	Sprite sPlayerDead2;
	sf::Sound aJump;
	sf::Sound aPlunge;
	sf::Sound aBlaster;
	sf::Sound aHurt;
	sf::Sound aDying;
	sf::Sound aDead;
	sf::Sound aSwim;
	sf::Sound aCp;

	void	UpdateHealth(float newHealth);
	float	Life() { return health; }

	int		GetID()		{ return EID_PLAYER; }
	bool	IsPlayer()	{ return true; }

	void	Step();
	void	CheckTiles(SolidMap *sldMap);
	void	PostStep();
	void	Draw();
	void	Collision(Entity *with);

	void	CallTimer(int num);

	void	LockInput();
	void	UnlockInput();
	void	PlayDeathSequence();
	void	PlayDeathSequence2();

	void	Hurt(float hp);
	float	GetHealth() { return health; }
	void	SetHealth(float hp) { UpdateHealth(hp); }
protected:
	void MoveLeft();
	void MoveRight();
	void Idle();
	void Jump();
};

#endif // E_PLAYER_H
