#pragma once
#ifndef GROOVYIZER_H
#define GROOVYIZER_H

#include <list>
#include "ChainedParticles.h"
#include "LameParticle.h"
#include "FireParticle.h"

class Groovyizer
{
	std::list<FireParticle*> fire;
	std::list<ChainedParticle*> chains;
	std::list<LameParticle*> lamers;
	int prev;
	unsigned chainTexture;
public:
	bool Frame();

	Groovyizer();
	~Groovyizer();
};

#endif // GROOVYIZER_H
