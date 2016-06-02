#include "E_LazerBeam.h"
#include "Engine.h"

extern Engine *eng;

E_LazerBeam::E_LazerBeam(int dir, bool enemyBeam)
{
	if (dir)
		hspeed = 3.0f;
	else
		hspeed = -3.0f;

	colRect.SetW(40.0f);
	colRect.SetH(4.0f);

	r=255;
	g=0;
	b=0;

	enemy = enemyBeam;

	SetTimer(0, 30);
}

void E_LazerBeam::ShortenLife(int amount)
{
	SetTimer(0, max(GetTimer(0) - amount, 1));
}

void E_LazerBeam::CallTimer(int num)
{
	switch (num)
	{
	case 0:
		destroy = true;
		break;
	}
}

void E_LazerBeam::Draw()
{
	eng->render->SetColor(r,g,b);
	eng->render->DrawRect(x, y, 40.0f, 4.0f);
}

void E_LazerBeam::Step()
{
	Entity::Step();

	colRect.SetX(x);
	colRect.SetY(y);
}

void E_LazerBeam::Collision(Entity *with)
{
	//Entity::Collision(with); // commented out: skip the rules
	// todo: destroy if colliding with stuff
}
