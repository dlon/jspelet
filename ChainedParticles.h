#pragma once
#ifndef CHAINED_PARTICLES_H
#define CHAINED_PARTICLES_H

struct PropagationInfo
{
	float x,y;
	int spawn; // particles left; one particle carries one child (i.e., spawn--)
	float speed; // actually velocity
	float dir; // in rads
	float friction;
	int bachelorTime; // delay before duplication
	float life;
	float r,g,b;
	bool randDir;
};

class ChainedParticle
{
	float x,y;
	float life;
	
	ChainedParticle *child;
	PropagationInfo pInfo;

	int update;
	int initialTime;
	bool isFirst;
public:
	ChainedParticle(/*float x, float y,*/ PropagationInfo *pInfo, bool first=true);
	~ChainedParticle();

	void Update();
	void Draw(float scale=1.0f);

	bool ChainAlive();
};

#endif // CHAINED_PARTICLES_H
