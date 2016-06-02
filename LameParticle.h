#pragma once



class LameParticle
{
	float x,y;
	float r,g,b;
	float scale;
	float life;
	int prev;
public:
	void Draw();
	bool Dead();
	void Update();

	LameParticle(float x,float y,float r,float g,float b);
};
