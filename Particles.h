#pragma once
#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>

struct ParticleDescriptor
{
	enum { X = 0, Y = 1 };

	float	pos[2];				/* X = 0, Y = 1 */
	float	dir;
	float	vel;
	float	frict;
	float	grav;
	float	gravDir;			/* pi0.5 */
	float	life;
	float	r[3],g[3],b[3];
	float	alpha[3];
	float	dup;				/* new particles to spawn each step	-- i > 0.0 */
	int		blending;			/* 0 = normal, 1 = additive */

	bool LoadDescFile(const char *path);

	void SetColor(float _r, float _g, float _b) {
		r[0] = r[1] = r[2] = _r;
		g[0] = g[1] = g[2] = _g;
		b[0] = b[1] = b[2] = _b;
	}
	void SetColor2v(float _r[2], float _g[2], float _b[2]) /* interpolate between 2 colors */
	{
		r[0] = _r[0]; r[1] = (_r[0]+_r[1])*0.5f; r[2] = _r[1];
		g[0] = _g[0]; g[1] = (_g[0]+_g[1])*0.5f; g[2] = _g[1];
		b[0] = _b[0]; b[1] = (_b[0]+_b[1])*0.5f; b[2] = _b[1];
	}
	void SetColor3v(float _r[3], float _g[3], float _b[3]) /* interpolate between 3 colors */
	{
		r[0] = _r[0]; r[1] = _r[1]; r[2] = _r[2];
		g[0] = _g[0]; g[1] = _g[1]; g[2] = _g[2];
		b[0] = _b[0]; b[1] = _b[1]; b[2] = _b[2];
	}
	void SetAlpha(float _a) {
		alpha[0] = alpha[1] = alpha[2] = _a;
	}
	void SetAlpha2v(float _a[2]) {
		alpha[0] = _a[0];
		alpha[1] = (_a[0]+_a[1])*0.5f;
		alpha[2] = _a[1];
	}
	void SetAlpha3v(float _a[3]) {
		alpha[0] = _a[0]; alpha[1] = _a[1]; alpha[2] = _a[2];
	}
};

struct ParticleForce	/* attractor-deflector */
{
	float	x,y;
	float	radius;
	float	force;
	int		type;		/* 0 = linear, 1 = constant, 2 = quadratic */
};

class ParticleEmitter
{
public:
	ParticleEmitter();

	void		Burst();
	void		Stream();
};

class ParticleSystem
{
	static const int ReserveSize = 50; /* reserve x emitters at a time */
public:
	// emitters
	std::vector<ParticleEmitter>	emitters;
	// forces
	std::vector<ParticleForce>		forces;

	ParticleEmitter		*CreateEmitter();


	ParticleSystem();
};

#endif // PARTICLES_H
