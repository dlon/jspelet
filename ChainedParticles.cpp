#include "ChainedParticles.h"
#include "Engine.h"
#include <math.h>
#include <GL/gl.h>
#include <stdio.h>

extern Engine *eng;

inline float randFloat(float largest)
{
	float x = float(rand())/1000.0f;
	return fmod(x, largest);
}

ChainedParticle::ChainedParticle(/*float x, float y, */PropagationInfo *_pInfo, bool first) : child(0)
{
	pInfo = *_pInfo;

	this->x = pInfo.x;
	this->y = pInfo.y;
	isFirst = first;
	life = pInfo.life;

	// update propagation
	if (!first)
		pInfo.spawn--;
	update = 0;
	initialTime = eng->time;

	// random direction
	if (pInfo.randDir)
		pInfo.dir = randFloat(6.28f);
}

ChainedParticle::~ChainedParticle()
{
	delete child;
}

void ChainedParticle::Update()
{
	// spawn child
	if (!child && pInfo.spawn)
	{
		if (eng->time - initialTime >= pInfo.bachelorTime)
			child = new ChainedParticle(/*x, y,*/ &pInfo, false);
	}

	// update particle & propagation
	if (update - eng->time > 15)
	{
		//x += pInfo.speed * cos(pInfo.dir);
		//y += pInfo.speed * sin(pInfo.dir);
		pInfo.x += pInfo.speed * cos(pInfo.dir);
		pInfo.y += pInfo.speed * sin(pInfo.dir);
		life -= 1.0f;
		pInfo.speed -= pInfo.friction;
		if (pInfo.speed < .0f)
			pInfo.speed = 0;
		if (life < 0.0f)
			life = 0;
		update = eng->time;
	}

	// update child
	if (child)
		child->Update();

	/*
	if (eng->time >= initialTime + pInfo.bachelorTime)
		delete this; // hmm, we now lose track of it!
	*/
}

void ChainedParticle::Draw(float scale)
{
	if (isFirst)
	{
		//glLineWidth(3.0f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		//glBegin(GL_LINE_STRIP);
		glBegin(GL_QUAD_STRIP);
	}

	if (life > .0f)
	{
		glColor4f(pInfo.r,pInfo.g,pInfo.b,life/pInfo.life);
		glVertex2f(x, y);
		glVertex2f(x + (cos(abs(pInfo.dir))*-3.0f)*scale, y + (sin(abs(pInfo.dir))*3.0f)*scale);
	}
	if (child)
		child->Draw(scale);
	
	if (isFirst)
	{
		glEnd();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

bool ChainedParticle::ChainAlive()
{
	if (child)
		return child->ChainAlive();
	return life > .0f;
}
