#pragma once
#ifndef PARTICLES_S_H
#define PARTICLES_S_H

#include <map>
#include <unordered_map>
#include <GL/gl.h>
#include "../Renderer.h"

struct Particle
{
	float x,y;
	float prevx,prevy;
	float dir,vel;
	float yvel,grav;
	float life;
	float scale;
	float r,g,b,a; // int color
};

enum EmitterFlags {
	EMP_NONE,
};

struct EmitterProperties {
	float			rx,ry; // distribution
	float			vmin,vmax; // velocity
	float			dmin,dmax; // direction
	float			rmin,gmin,bmin,amin; // rgb
	float			rmax,gmax,bmax,amax;
	float			lmin,lmax; // life
	float			smin,smax; // scale
	bool			tail;
	int				rate; // particles/sec
	int				blending;
	bool			stream;
	Texture *		texture;
	EmitterFlags	flags;
	float			grmin,grmax; // gravity: should in principle be reduced to mass, as should almost all elements in this struct
};

// particle mng
class ParticleEmitter;

class ParticleManager
{
	std::unordered_map<const char *, ParticleEmitter*>	emitters;			// std::string?
	std::multimap<int, ParticleEmitter*>				orderedEmitters;	// depth
public:
	~ParticleManager();

	void SetDepthValue(ParticleEmitter *pe, int depth); // depth: background > 0 > foreground

	ParticleEmitter *		GetEmitter(const char *name) const;
	ParticleEmitter *		NewEmitter(const char *name,
		float x, float y, int partMax=50, EmitterProperties *prop = 0);	// depth = 0
	ParticleEmitter *		NewEmitter(const char *name,
		int depth,
		float x, float y, int partMax=50, EmitterProperties *prop = 0);

	void Update() const;
	void Draw() const;
};

// texture loaders
struct PDefaultLoadTexture {
	unsigned int operator ()(const char *file);
};
struct PDefaultFreeTexture {
	void operator ()(unsigned int);
};

// emitter
class ParticleEmitter
{
	//int gl_tex;
	int anim; // >0 = animated
	unsigned *animTex;
	float animFrame;
	float animSpeed;
	bool animLoop;
	int lElapse;

protected:
	void RecalcBounds();

public:
	float x,y;
	EmitterProperties prop;
	
	Particle *particles;
	int limit;

	void Emit(); // stream = false
	void Update();
	void Draw();
	bool PosFree(float x, float y);
	int CountParticles();
	void LoadConfiguration(const char *part_file);
	void DeflectAtBounds(float x1, float y1, float x2, float y2);
	//void SetSequence(int num, unsigned *tex) { ; }
	void SetAnim(int num, unsigned *tex);
	void UnsetAnim();
	void AnimProp(float speed, bool loop) { animSpeed=speed; animLoop=loop; }

	void SetProperties(EmitterProperties *prop);

	void		SetTexture(Texture *t)					{ prop.texture = t; }
	Texture*	GetTexture()							{ return prop.texture; }
	void		UnsetTexture()							{ prop.texture = 0; }

#ifndef NDEBUG
	void DInfo();
#endif

	ParticleEmitter(float _x, float _y,
		int pMax = 50, EmitterProperties *prop = 0 /* use default */);
	~ParticleEmitter();
};

#endif // PARTICLES_S_H
