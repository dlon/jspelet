#pragma once

struct FirePropagationInfo
{
	float x,y;
	int spawn; // particles left; one particle carries one child (i.e., spawn--)
	float speed; // actually velocity
	float dir; // in rads
	float friction;
	int bachelorTime; // delay before duplication
	float life;
};

class FireParticle
{
	float x,y;
	float life;
	
	FireParticle *child;
	FirePropagationInfo pInfo;

	int update;
	int initialTime;
	bool isFirst;
public:
	FireParticle(FirePropagationInfo *pInfo, bool first=true);
	~FireParticle();

	void Update();
	void Draw(float scale=1.0f);

	bool ChainAlive();

	int CountLength(int i=0);
};
