#pragma once
#include <stdio.h>

class ChaseCam
{
	int explEnd;
	int explNext;
	float explOffX;
	float explOffY;
	float explIntensity;
	int explInterval;
public:
	float minX,minY;
	float x,y;
	float xvel,yvel;
	float mx,my;
	float hRect,vRect;
	bool reset;

	void MoveTo(float x, float y) {
		//printf("mx: %f\n", x);
		mx = x > minX ? x : minX;
		my = y > minY ? y : minY;
	}
	void Update();
	void Reset() { reset=true; }
	
	void Explode();
	void EarthQuake();
	
	// transitions?

	ChaseCam() : explNext(0), minX(0), minY(0), explEnd(0), explOffX(0), explOffY(0), explInterval(0), explIntensity(0), x(0), y(0), mx(0), my(0), xvel(0), yvel(0), hRect(40.0f), vRect(40.0f) {}
};
