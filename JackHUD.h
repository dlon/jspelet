#pragma once
#ifndef JACKHUD_H
#define JACKHUD_H

#include "Sprite.h"

class JackPlayerInfo
{
public:
	virtual float GetHealth() = 0;
	virtual void SetHealth(float hp) = 0;

	virtual ~JackPlayerInfo() {}
};
class BossInfo
{
protected:
	float healthBarX,healthBarW;
	float healthBarY,healthBarH;
	float healthBarCur;
public:
	virtual float	GetMaxHealth() = 0;
	virtual float	GetHealth() = 0;
	virtual void	SetHealth(float hp) = 0;

	virtual void	HudStep();
	virtual void	HudDraw();

	BossInfo();
	virtual ~BossInfo() {}
};

class JackHUD
{
	float	health;
	float	healthMax;
	float	healthBarX,healthBarW;
	float	healthBarY,healthBarH;
	float	healthBarCur;
	bool	lowHealthSign;
	bool	lowHealthMeansCheckPoint;
	float	lowHealthA;
	bool	lowSubtract;
	int		lowHealthFrames;

	JackPlayerInfo	*player;
	BossInfo		*boss;
public:
	int		heartContainers;
	Sprite	hierta;

	JackHUD();

	void			SetPlayer(JackPlayerInfo *jpi) { player = jpi; }
	JackPlayerInfo *GetPlayer() { return player; }

	void			SetBoss(BossInfo *bi);
	BossInfo		*GetBoss() { return boss; }

	void CheckPoint();

	void Step();
	void Draw();
	void OneUp();
	void OneDown();

	int GetLives();
	void SetLives(int lives);
};

#endif // HUD_H
