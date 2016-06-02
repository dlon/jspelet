#include "particles.h"
#include "ini.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glext.h>

#include "../Engine.h"
#include "../Renderer.h"

extern Engine *eng;

#ifndef M_PI
#define M_PI 3.1415926f
#endif // M_PI

EmitterProperties defaultEmitter = {
	0.0f, 0.0f, // distribution
	0.0f, 0.0f, // velocity
	0.0f, 0.0f, // direction
	0.5f, 0.5f, 0.5f, 1.0f, // color
	0.5f, 0.5f, 0.5f, 1.0f,
	5.0f, 5.0f, // lifespan
	1.0f, 1.0f,
	false, // tail
	10, // rate
	0, // blend mode
	true,
	0,
	EMP_NONE,
	0.0f, 0.0f, // gravity
};

static int GetRandValue(int min, int max)
{
	if (min > max)
	{
		min ^= max;
		max ^= min;
		min ^= max;
	}
	return min + (rand() % (max-min+1));
}

static float GetRandValuef(float min, float max)
{
	if (min > max)
	{
		float t = max;
		max = min;
		min = t;
	}
	return (rand()/(RAND_MAX + 1.0f)) * (max-min) + min;
}

ParticleEmitter::ParticleEmitter(float _x, float _y,
								 int pMax,
								 EmitterProperties *prop)
								 : x(_x), y(_y), limit(pMax),
								 //gl_tex(-1),
								 anim(0),
								 animTex(0),
								 animFrame(0),
								 lElapse(0)
{
	if (prop)
	{
		this->prop = *prop;
	}
	else {
		// default
		this->prop = defaultEmitter;
	}

	particles = new Particle[limit];
	for (int i=0; i<limit; i++)
		particles[i].life = 0.0f;
}

ParticleEmitter::~ParticleEmitter()
{
	delete [] particles;
	UnsetAnim();
}

void ParticleEmitter::Emit()
{
	if (++lElapse >= int(60.0/(double)prop.rate)) // TODO: More accuracy. FIXME: maximum is 60 (60/60=1)
	{
		for (int i=0; i<limit; i++)
		{
			if (particles[i].life <= 0.0f)
			{
				particles[i].x = x + GetRandValuef(0, prop.rx) - prop.rx / 2.0f;
				particles[i].y = y + GetRandValuef(0, prop.ry) - prop.ry / 2.0f;
				particles[i].prevx = particles[i].x;
				particles[i].prevy = particles[i].y;
				particles[i].dir = GetRandValuef(prop.dmin, prop.dmax);
				particles[i].vel = GetRandValuef(prop.vmin, prop.vmax);
				particles[i].grav = GetRandValuef(prop.grmin, prop.grmax);
				particles[i].life = GetRandValuef(prop.lmin, prop.lmax);
				particles[i].r = GetRandValuef(prop.rmin, prop.rmax);
				particles[i].g = GetRandValuef(prop.gmin, prop.gmax);
				particles[i].b = GetRandValuef(prop.bmin, prop.bmax);
				particles[i].a = GetRandValuef(prop.amin, prop.amax);
				particles[i].scale = GetRandValuef(prop.smin, prop.smax);
				particles[i].yvel = 0; // special for gravity (for now)

				break;
			}
		}
		//hrt.Reset();
		lElapse -= int(60.0/(double)prop.rate);
	}
}

void ParticleEmitter::Update()
{
	// animate ( do per particle! )
	if (anim > 0)
	{
		if (animLoop || ((int)animFrame)+1 < anim)
			animFrame = fmod(animFrame+animSpeed, (float)anim);
		//gl_tex = animTex[(int)animFrame]; // slow
	}

	if (prop.stream)
		Emit();

	// update the particles
	for (int i=0; i<limit; i++)
	{
		if (particles[i].life > 0.0f)
		{
			particles[i].yvel += particles[i].grav; // reserved for gravity. TODO: Limit

			particles[i].prevx = particles[i].x;
			particles[i].prevy = particles[i].y;
			particles[i].x += particles[i].vel * cos( particles[i].dir * (M_PI/180.0f) );
			particles[i].y += particles[i].vel * sin( particles[i].dir * (M_PI/180.0f) ) + particles[i].yvel;
			particles[i].life -= 0.1f;

			// FIXME: (messes up if 1st alpha val isn't 1)
			particles[i].a = particles[i].life / prop.lmax;
		}
	}
}

void ParticleEmitter::Draw()
{
	switch (prop.blending)
	{
	case 0:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case 1:
	default:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	}

	if (prop.tail)
	{
		//if (gl_tex == -1)
		{
			glBegin(GL_LINES);
			for (int i=0; i<limit; i++)
			{
				if (particles[i].life > 0.0f)
				{
					glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].a );
					glVertex3f(particles[i].prevx, particles[i].prevy, 0.0f);
					glVertex3f(particles[i].x, particles[i].y, 0.0f);
				}
			}
			glEnd();
		}
	}
	else
	{
		//if (gl_tex == -1)
		if (!prop.texture)
		{
			glBegin(GL_POINTS);
			for (int i=0; i<limit; i++)
			{
				if (particles[i].life > 0.0f)
				{
					glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].a);
					glVertex3f(particles[i].x, particles[i].y, 0.0f);
				}
			}
			glEnd();
		}
		else
		{
			glEnable(eng->render->textureType);
			glBindTexture(eng->render->textureType, (unsigned)prop.texture->reserved);
			
			float tw = (float)prop.texture->w;
			float th = (float)prop.texture->h;

			float xw=1.0f, yw=1.0f;
			if (!eng->render->rectExt) {
				xw = float(prop.texture->w + prop.texture->paddingW);
				yw = float(prop.texture->h + prop.texture->paddingH);
			}

			glBegin(GL_QUADS);
			for (int i=0; i<limit; i++)
			{
				if (particles[i].life > 0.0f)
				{
					glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].a);
					glTexCoord2f(tw/xw, 0); // note: mirrored
					glVertex2f(particles[i].x-(32.0f*particles[i].scale),
						particles[i].y-(32.0f*particles[i].scale));
					glTexCoord2f(0, 0);
					glVertex2f(particles[i].x+(32.0f*particles[i].scale),
						particles[i].y-(32.0f*particles[i].scale));
					glTexCoord2f(0, th/yw);
					glVertex2f(particles[i].x+(32.0f*particles[i].scale),
						particles[i].y+(32.0f*particles[i].scale));
					glTexCoord2f(tw/xw, th/yw);
					glVertex2f(particles[i].x-(32.0f*particles[i].scale),
						particles[i].y+(32.0f*particles[i].scale));
				}
			}
			glEnd();
			glDisable(eng->render->textureType);
		}
	}
}

int ParticleEmitter::CountParticles()
{
	int r=0;
	for (int i=0; i<limit; i++)
	{
		if (particles[i].life > 0.0f)
			r++;
	}
	return r;
}

void ParticleEmitter::LoadConfiguration(const char *part_file)
{
	FILE *t = fopen(part_file, "rb");
	if (!t)
	{
		char temp[MAX_PATH+64];
		sprintf(temp, "Cannot read: %s", part_file);
		MessageBox(NULL, temp, "Configuration file", MB_ICONERROR);
	}
	fclose(t);

	// shape config
	const char *p;
	p = ini_read_str(part_file, "shape", "type", "point");
	// todo: texture

	// emitter config
	prop.rx = (float)ini_read_double(part_file, "emitter", "rx", 0);
	prop.ry = (float)ini_read_double(part_file, "emitter", "ry", 0);
	prop.vmin = (float)ini_read_double(part_file, "emitter", "vel_min", 0);
	prop.vmax = (float)ini_read_double(part_file, "emitter", "vel_max", 0);
	prop.dmin = (float)ini_read_double(part_file, "emitter", "dir_min", 0);
	prop.dmax = (float)ini_read_double(part_file, "emitter", "dir_max", 0);
	prop.rmin = (float)ini_read_double(part_file, "emitter", "color_min_r", 0);
	prop.rmax = (float)ini_read_double(part_file, "emitter", "color_max_r", 0);
	prop.gmin = (float)ini_read_double(part_file, "emitter", "color_min_g", 0);
	prop.gmax = (float)ini_read_double(part_file, "emitter", "color_max_g", 0);
	prop.bmin = (float)ini_read_double(part_file, "emitter", "color_min_b", 0);
	prop.bmax = (float)ini_read_double(part_file, "emitter", "color_max_b", 0);
	prop.amin = (float)ini_read_double(part_file, "emitter", "alpha_min", 0);
	prop.amax = (float)ini_read_double(part_file, "emitter", "alpha_max", 0);
	prop.lmin = (float)ini_read_double(part_file, "emitter", "life_min", 0);
	prop.lmax = (float)ini_read_double(part_file, "emitter", "life_max", 0);
	prop.smin = (float)ini_read_double(part_file, "emitter", "scale_min", 1.0);
	prop.smax = (float)ini_read_double(part_file, "emitter", "scale_max", 1.0);
	prop.tail = ini_read_int(part_file, "emitter", "tail", 0) != 0;
	prop.rate = ini_read_int(part_file, "emitter", "rate", 0);
	prop.blending = ini_read_int(part_file, "emitter", "blending", 0);
	prop.stream = ini_read_int(part_file, "emitter", "stream", 1) != 0;
}

bool ParticleEmitter::PosFree(float x, float y)
{
	return 1;
}

void ParticleEmitter::DeflectAtBounds(float x1, float y1,
									  float x2, float y2)
{
	/*for (int i=0; i<limit; i++)
	{
		if (particles[i].life > 0.0f)
		{
			if (particles[i].x >= x1 &&
				particles[i].x+5 < x2 &&
				particles[i].y >= y1 &&
				particles[i].y+5 < y2)
			{
				//particles[i].x = particles[i].prevx;
				//particles[i].y = particles[i].prevy;
				//particles[i].dir = fmod(particles[i].dir+90.0f, 360.0f);
			}
		}
	}*/
}

void ParticleEmitter::SetAnim(int num, unsigned *tex)
{
	if (num > 0)
	{
		animFrame = 0;
		anim = num;
		delete [] animTex;
		animTex = new unsigned[num];
		memcpy(animTex, tex, num*sizeof(unsigned));
		animLoop = true;
		animSpeed = 0.25f;
	}
	else
		UnsetAnim();
}

void ParticleEmitter::UnsetAnim()
{
	animFrame = 0;
	anim = 0;
	delete [] animTex;
	animTex = NULL;

	UnsetTexture();
}

void ParticleEmitter::SetProperties(EmitterProperties *prop)
{
	if (prop) {
		this->prop = *prop;
	}
	else {
		// default
		this->prop = defaultEmitter;
	}
}

// mng

#include <algorithm>

struct DelPtr {
	void operator ()(std::pair<const char *, ParticleEmitter*> p) { delete p.second; }
};

void ParticleManager::Update() const
{
	/*
	stdext::hash_map<const char*,ParticleEmitter*>::const_iterator it = emitters.begin();
	for (; it != emitters.end(); ++it)
		it->second->Update();
	*/
	// depth order
	std::multimap<int, ParticleEmitter*>::const_iterator it = orderedEmitters.begin();
	for (; it != orderedEmitters.end(); ++it)
		it->second->Update();
}

void ParticleManager::Draw() const
{
	/*
	stdext::hash_map<const char*,ParticleEmitter*>::const_iterator it = emitters.begin();
	for (; it != emitters.end(); ++it)
		it->second->Draw();
	*/
	// draw in depth order
	std::multimap<int, ParticleEmitter*>::const_iterator it = orderedEmitters.begin();
	for (; it != orderedEmitters.end(); ++it) {
		it->second->Draw();
	}
}

ParticleEmitter* ParticleManager::NewEmitter(const char *name,
											 float x, float y, int partMax, EmitterProperties *prop)
{
	return NewEmitter(name,0,x,y,partMax,prop);
}

ParticleEmitter* ParticleManager::NewEmitter(const char *name,
											 int depth,
											 float x, float y, int partMax, EmitterProperties *prop)
{
	ParticleEmitter *pe = new ParticleEmitter(x, y, partMax, prop);
	emitters[name] = pe;
	
	orderedEmitters.insert(std::pair<int,ParticleEmitter*>(depth, pe)); // depth list

	return pe;
}

ParticleEmitter * ParticleManager::GetEmitter(const char *name) const
{
	std::unordered_map<const char*,ParticleEmitter*>::const_iterator it = emitters.find(name); // FIXME-SFML: switched from hash_map
	if (it == emitters.end())
		return 0;
	return it->second;
}

ParticleManager::~ParticleManager()
{
	std::for_each(emitters.begin(), emitters.end(), DelPtr());
}

//
// sorted map
//
void ParticleManager::SetDepthValue(ParticleEmitter *pe, int depth)
{
	//std::sort(orderedEmitters.begin(), orderedEmitters.end(), SortDepthOrder());
	std::multimap<int,ParticleEmitter*>::iterator it = orderedEmitters.begin(); // FIXME: optimization? direct reference from hash map?

	// remove duplicates. TODO: a faster solution?
	for (; it != orderedEmitters.end(); ++it) {
		if (it->second == pe)
			orderedEmitters.erase(it);
	}
}
