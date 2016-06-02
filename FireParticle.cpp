#include "FireParticle.h"
#include "Engine.h"
#include <math.h>
#include <GL/gl.h>
#include <stdio.h>

#define COL_R 0.225f
#define COL_G 0.07f
#define COL_B 0.02f

extern Engine *eng;

static void DrawCircle(float x, float y, float r)
{
	float rad = 0;

	glBegin(GL_TRIANGLE_STRIP);
	while (rad < 6.5f)
	{
		glVertex2f(x,y);
		glVertex2f(x + cos(rad)*r, y + sin(rad)*r);
		rad += 0.314f;
	}
	glEnd();
}

inline float randFloat(float largest)
{
	float x = float(rand())/1000.0f;
	return fmod(x, largest);
}

FireParticle::FireParticle(FirePropagationInfo *_pInfo, bool first) : update(0), child(0)
{
	pInfo = *_pInfo;

	this->x = pInfo.x;
	this->y = pInfo.y;
	isFirst = first;
	life = pInfo.life;

	pInfo.dir += -0.3f + randFloat(0.6f);

	// update propagation
	if (!first)
		pInfo.spawn--;
	initialTime = eng->time;
}

FireParticle::~FireParticle()
{
	delete child;
}

void FireParticle::Update()
{
	// spawn child
	if (!child && pInfo.spawn)
	{
		if (eng->time >= initialTime + pInfo.bachelorTime)
			child = new FireParticle(&pInfo, false);
	}

	// update particle & propagation
	if (eng->time - update > 15)
	{
		x += pInfo.speed * cos(pInfo.dir);
		y += pInfo.speed * sin(pInfo.dir);
		pInfo.x = x;
		pInfo.y = y;

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

void FireParticle::Draw(float scale)
{
	if (isFirst)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	if (life > .0f)
	{
		/*
		glColor4f(pInfo.r,pInfo.g,pInfo.b,life/pInfo.life);
		glVertex2f(x, y);
		glVertex2f(x + (cos(abs(pInfo.dir))*-3.0f)*scale, y + (sin(abs(pInfo.dir))*3.0f)*scale);
		*/
		
		// draw circle
		if (life > pInfo.life/2.0f)
			glColor4f(COL_R,COL_G,COL_B,1.0f - life/pInfo.life);
		else
			glColor4f(COL_R,COL_G,COL_B,life/pInfo.life);
		glPushMatrix();
		glTranslatef(x,y,0.0f);
		glScalef(scale*life/pInfo.life, scale*life/pInfo.life, 1.0f);
		for (float r=1.0f; r<20.0f; r+=2.0f)
			DrawCircle(0.0f,0.0f,r);
		glPopMatrix();
	}
	if (child)
		child->Draw(scale);
	
	if (isFirst)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool FireParticle::ChainAlive()
{
	if (child)
		return child->ChainAlive();
	return life > .0f;
}

int FireParticle::CountLength(int i)
{
	if (child)
		return child->CountLength(++i);
	return i;
}
