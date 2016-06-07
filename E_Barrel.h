#pragma once

#include "Entity.h"
#include "particles/particles.h"

class E_Barrel : public Entity
{
	ParticleEmitter *em1;
	ParticleEmitter *em2;
	ParticleEmitter *em3;
	ParticleEmitter *em4;
	ParticleEmitter *em5; // blood
	ParticleEmitter *curFire;
	ParticleEmitter *curSmoke;
	bool pl;
	bool deactivated;
	bool solid;
	float dmg;

	Sound					sndFire;
	Sound						sndExplo;
	Entity						*player;
public:
	bool		onPlatform;
	float		platformH;
	float		platformV;

	E_Barrel();
	~E_Barrel();

	int		GetID() { return EID_BARREL; }
	void	Step();
	void	CallTimer(int num);

	//bool	IsPushable() { return true; }
	bool	IsSolid() { return solid; }

	void	Collision(Entity *with);

	Sprite sprite;
};
