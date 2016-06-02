#include "LameParticle.h"
#include "Engine.h"
#include <GL/gl.h>

#define FULL_LIFE_SPAN 40.0f

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

LameParticle::LameParticle(float x,float y,float r,float g,float b) : prev(0)
{
	this->x = x;
	this->y = y;
	this->r = r;
	this->g = g;
	this->b = b;
	this->scale = 1.5f;
	this->life = FULL_LIFE_SPAN;
}

void LameParticle::Update()
{
	if (eng->time - prev > 15)
	{
		life -= 1.0f;
		if (life < .0f)
			life = 0;
		prev = eng->time;
	}
}

bool LameParticle::Dead()
{
	return life <= .0f;
}

void LameParticle::Draw()
{
	if (life > FULL_LIFE_SPAN/2.0f)
		glColor4f(r,g,b,1.0f - life/FULL_LIFE_SPAN);
	else
		glColor4f(r,g,b,life/FULL_LIFE_SPAN);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glPushMatrix();
	glTranslatef(x,y,0.0f);
	glScalef(scale*life/FULL_LIFE_SPAN, scale*life/FULL_LIFE_SPAN, 1.0f);

	for (float r=1.0f; r<20.0f; r+=2.0f)
		DrawCircle(0.0f,0.0f,r);

	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
