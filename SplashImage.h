#pragma once
#ifndef STILLIMAGE_H
#define STILLIMAGE_H

#include "Renderer.h"

#define SPLASH_W 640
#define SPLASH_H 480

class SplashImage {
	sf::Texture *bg;
	//int MapPauseStatePrev;
	bool isActive;

	int x;
	int y;

	bool hide;
public:
	float alpha;
	int timeOutTicks;

	SplashImage();
	SplashImage(sf::Texture *tex);
	~SplashImage();

	void SetImage(sf::Texture *tex);
	bool HasTexture() { return bg != NULL; }

	void Show();
	void Hide();
	void Toggle();

	void Update();
	void Draw();

	bool IsActive() { return isActive; }
};

#endif // STILLIMAGE_H
