#include "Groovyizer.h"
#include "Engine.h"
#include "ChainedParticles.h"
#include <math.h>

#define SCREEN_W 640
#define SCREEN_H 480

extern Engine *eng;

inline float randFloat(float largest)
{
	float x = float(rand())/1000.0f;
	return fmod(x, largest);
}

Groovyizer::Groovyizer() : prev(0)
{
	//chainTexture = eng->res->textures.Load("data/chain.png")->reserved;
}

Groovyizer::~Groovyizer()
{
	{
		std::list<ChainedParticle*>::iterator it = chains.begin();
		for (; it != chains.end(); it++)
			delete *it;
	}
	{
		std::list<LameParticle*>::iterator it = lamers.begin();
		for (; it != lamers.end(); it++)
			delete *it;
	}
	{
		std::list<FireParticle*>::iterator it = fire.begin();
		for (; it != fire.end(); it++)
			delete *it;
	}

	//eng->res->textures.FreeByFile("data/chain.png");
}

bool Groovyizer::Frame()
{
	if (eng->time - prev > 15)
	{
		// new chain
#if 0
		PropagationInfo prop;
		prop.x = rand() % SCREEN_W;
		prop.y = rand() % SCREEN_H;
		prop.spawn = 100;
		prop.speed = 3.5f;
		prop.dir = randFloat(3.14f * 2.0f);
		prop.friction = 0.0000f;
		prop.bachelorTime = 10;
		prop.life = 25.0f;
		prop.r = randFloat(1.0f);
		prop.g = randFloat(1.0f);
		prop.b = randFloat(1.0f);
		prop.randDir = true;
		chains.push_back( new ChainedParticle(/*30.0f, 30.0f,*/ &prop) );
#endif

		// test
		FirePropagationInfo fprop;
		fprop.x = (SCREEN_W/2);
		fprop.y = SCREEN_H/4*3;
		//fprop.x = rand()%SCREEN_W;
		//fprop.y = SCREEN_H;
		fprop.spawn = 6;
		fprop.speed = 1.5f;
		fprop.dir = /*randFloat(3.14f * 2.0f)*/-1.55f;
		fprop.friction = 0.0000f;
		fprop.bachelorTime = 100;
		fprop.life = 25.0f;
		//fprop.life = 50.0f;
		fire.push_back(new FireParticle(&fprop));

		// new lamer
		//for (int i=0; i<3; i++)
		//	lamers.push_back(new LameParticle((float)(rand()%SCREEN_W),(float)(rand()%SCREEN_H),randFloat(1.0f),randFloat(1.0f),randFloat(1.0f)));

		prev = eng->time;
	}

	std::list<ChainedParticle*>::iterator it;
	std::list<LameParticle*>::iterator lit;
	std::list<FireParticle*>::iterator fit;

	// eliminate dead fire
	for (fit = fire.begin(); fit != fire.end(); )
	{
		std::list<FireParticle*>::iterator next = fit;
		next++;
		if ( !(*fit)->ChainAlive() )
		{
			delete *fit;
			fire.erase(fit);
		}
		fit = next;
	}

	// eliminate dead chains
	for (it = chains.begin(); it != chains.end(); )
	{
		std::list<ChainedParticle*>::iterator next = it;
		next++;
		if ( !(*it)->ChainAlive() )
		{
			delete *it;
			chains.erase(it);
		}
		it = next;
	}

	// eliminate dead lamers
	for (lit = lamers.begin(); lit != lamers.end();)
	{
		std::list<LameParticle*>::iterator next = lit;
		next++;
		if ((*lit)->Dead())
		{
			delete *lit;
			lamers.erase(lit);
		}
		lit = next;
	}

	// update
	for (it = chains.begin(); it != chains.end(); it++)
		(*it)->Update();
	for (lit = lamers.begin(); lit != lamers.end(); lit++)
		(*lit)->Update();
	for (fit = fire.begin(); fit != fire.end(); fit++)
		(*fit)->Update();

	eng->render->BeginFrame();
	/*
	for (lit = lamers.begin(); lit != lamers.end(); lit++)
		(*lit)->Draw();
	for (it = chains.begin(); it != chains.end(); it++)
	{
		(*it)->Draw();
		//(*it)->Draw(1.2f);
		//(*it)->Draw(1.8f);
	}
	//*/
	for (fit = fire.begin(); fit != fire.end(); fit++)
		(*fit)->Draw();
	eng->render->EndFrame();

	return true;
}
