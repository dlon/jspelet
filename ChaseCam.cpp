#include "ChaseCam.h"
#include <math.h>
#include "Engine.h"
#include <stdio.h>

extern Engine *eng;

void ChaseCam::Update()
{
	if (reset) {
		minX = 0; // not sure here
		minY = 0;
		x = mx;
		y = my;
		reset = false;
	}

	if (mx < minX)
		mx = minX;
	if (my < minY)
		my = minY;

	//if (abs(mx-x) > hRect)
	{
		xvel = (mx-x)/15.0f;
		x += xvel;
	}
	//if (abs(my-y) > vRect)
	{
		yvel = (my-y)/15.0f;
		y += yvel;
	}

	eng->render->cam.x = x + explOffX;
	eng->render->cam.y = y + explOffY;

	if (eng->time < explEnd) {
		if (eng->time >= explNext) {
			explIntensity *= 0.8f;
			explOffX = float(rand()%26 - 13) * explIntensity;
			explOffY = float(rand()%26 - 13) * explIntensity;

			explNext = eng->time + explInterval;
		}
	}
	else {
		explOffX = 0;
		explOffY = 0;
	}
}

void ChaseCam::EarthQuake() {
	explInterval = 50;
	explIntensity = 1.0f;
	explEnd = eng->time + 3000;
	explNext = eng->time + explInterval;
}

void ChaseCam::Explode() {
	explInterval = 20;
	explIntensity = 1.0f;
	explEnd = eng->time + 500;
	explNext = eng->time + explInterval;
}
