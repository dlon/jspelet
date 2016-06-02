#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include <cmath>
#include "Renderer.h"

class Sprite
{
	SubImage *img;
	int imgNum;
	bool flip;
public:
	// sprite properties
	float imgInd;
	float imgSpeed;
	float xscale;
	float yscale;
	float rot;
	float xofs;
	float yofs;
	float x,y;

	int GetNumFrames() { return imgNum; }
	int GetHeight() { return img[(int)imgInd].h; }
	int GetWidth() { return img[(int)imgInd].w; }

	SubImage * GetFrame(int i) { return &img[i]; }
	SubImage * operator[](int i) { return &img[i]; }

	// free-pos ver
	void SetSprite(int num, SubImage *images,
		float _xscale = 1.0f, float _yscale = 1.0f);

	// symmetric ver (x-pane)
	void SetSprite(SubImage *first, int num,
		float _xscale = 1.0f, float _yscale = 1.0f);

	// clean version
	void Load(const char *file, int frames, int w, int h, int x=0, int y=0);
	void Load(Texture *t, int frames, int w, int h, int x=0, int y=0);
	void LoadHorizontal(Texture *t, int frames, int w, int h, int x=0, int y=0);

	void Draw();
	void DrawEx(float x, float y, float tx, float ty, float w, float h); // x,y=map coordinates --- w,y=surface w (not scale) --- tx,ty=subimg offset, not abs tex pos
	void Update();
	bool Collision(Sprite *other);
	bool Collision(SubImage *other, float x, float y);

	void NormalFlip();
	void MirrorFlip();
	bool Flipped() { return flip; }
	void Flip();

	Sprite(SubImage *first, int num) : x(0), y(0), img(0), flip(false) { SetSprite(first, num); }
	Sprite() : x(0), y(0), img(0), flip(false) {}
	Sprite(const char *file, int frames, int w, int h, int x=0, int y=0);

	~Sprite() { delete [] img; }
};

#endif // SPRITE_H
