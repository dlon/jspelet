#include "Sprite.h"
#include <assert.h>
#include "Engine.h"

extern Engine *eng;

Sprite::Sprite(const char *file, int frames, int w, int h, int _x, int _y) : img(0), x(0), y(0), flip(false)
{
	Load(file,frames,_x,_y,w,h);
}

void Sprite::Load(const char *file, int frames, int w, int h, int x, int y)
{
	Texture *t = eng->res->textures.Load(file); // load or get
	Load(t, frames, w, h, x, y);
}

void Sprite::Load(Texture *t, int frames, int w, int h, int x, int y)
{
	assert(t); // no texture

	delete[] img;
	img = new SubImage[frames];

	w = (!w ? t->w : w);
	h = (!h ? t->h : h);

	for (int i=0; i<frames; i++)
	{
		img[i].t=t;
		img[i].tx=x+i*w;
		img[i].ty=y;
		img[i].h=h;
		img[i].w=w;
	}

	// set members
	xscale = 1.0f;
	yscale = 1.0f;
	imgInd = 0;
	imgSpeed = 0;
	imgNum = frames;
	xofs = 0;
	yofs = 0;
	rot = 0;
}
void Sprite::LoadHorizontal(Texture *t, int frames, int w, int h, int x, int y)
{
	assert(t); // no texture

	delete[] img;
	img = new SubImage[frames];

	w = (!w ? t->w : w);
	h = (!h ? t->h : h);

	for (int i=0; i<frames; i++)
	{
		img[i].t=t;
		img[i].tx=x;
		img[i].ty=y+i*h;
		img[i].h=h;
		img[i].w=w;
	}

	// set members
	xscale = 1.0f;
	yscale = 1.0f;
	imgInd = 0;
	imgSpeed = 0;
	imgNum = frames;
	xofs = 0;
	yofs = 0;
	rot = 0;
}

void Sprite::SetSprite(int num, SubImage *images, float _xscale, float _yscale)
{
	// clone array
	delete[] img;
	img = new SubImage[num];
	
	for(int i=0; i<num; i++)
		img[i] = images[i];

	// reset members
	xscale = _xscale;
	yscale = _yscale;
	imgInd = 0;
	imgSpeed = 0;
	imgNum = num;
	xofs = 0;
	yofs = 0;
	rot = 0;
}

void Sprite::SetSprite(SubImage *first, int num, float _xscale, float _yscale)
{
	delete [] img;
	img = new SubImage[num];

	int x = first->tx;
	
	// make symmetric array
	for(int i=0; i<num; i++)
	{
		img[i].t = first->t;
		img[i].h = first->h;
		img[i].w = first->w;
		img[i].tx = x;
		img[i].ty = first->ty;
		x += first->w;
	}

	// reset members
	xscale = _xscale;
	yscale = _yscale;
	imgInd = 0;
	imgSpeed = 0;
	imgNum = num;
	xofs = 0;
	yofs = 0;
	rot = 0;
}

void Sprite::Update()
{
	if (imgSpeed > 0.0f)
		imgInd = std::fmod(imgInd + imgSpeed, float(imgNum));
	else {
		// reversed
		imgInd += imgSpeed;
		while (imgInd < 0.0f)
			imgInd += (float)imgNum-0.1f;
	}
}

void Sprite::Draw()
{
	if (img)
		eng->render->DrawSubImageEx(&img[(int)imgInd], x+xofs, y+yofs, rot, xscale, yscale);
}

void Sprite::DrawEx(float x, float y, float tx, float ty, float w, float h)
{
	if (img) {
		SubImage &sub = img[(int)imgInd];
		float rw=(float)sub.w, rh=(float)sub.h;
		float rx=(float)sub.tx, ry=(float)sub.ty;

		// don't exceed bounds
		if (w+tx > rw) return;
		if (h+ty > rh) return; // todo: clip instead of dismiss
		
		sub.tx = (int)rx+tx;
		sub.ty = (int)ry+ty;
		sub.w = w;
		sub.h = h;

		eng->render->DrawSubImageEx(&sub, x, y, rot, xscale, yscale);

		// rst w/h
		sub.w = (int)rw;
		sub.h = (int)rh;
		sub.tx = (int)rx;
		sub.ty = (int)ry;
	}
}

void Sprite::NormalFlip()
{
	if (flip && img)
	{
		xofs -= img[0].w;
		xscale = 1.0f;
		flip = false;
	}
}

void Sprite::MirrorFlip()
{
	if (!flip && img)
	{
		xofs += (float)img[0].w;
		xscale = -1.0f;
		flip = true;
	}
}

void Sprite::Flip() {
	if (!flip)
		MirrorFlip();
	else
		NormalFlip();
}

bool Sprite::Collision(Sprite *other)
{
	if (!img || !other->img)
		return false;
	if (x + img[(int)imgInd].w <= other->x) // x1<y0
		return false;
	if (x >= other->x + other->img[(int)other->imgInd].w) // x0>y1
		return false;
	if (y + img[(int)imgInd].h <= other->y) // x1<y0
		return false;
	if (y >= other->y + other->img[(int)other->imgInd].h) // x0>y1
		return false;
	return true;
}

bool Sprite::Collision(SubImage *other, float _x, float _y)
{
	if (!img)
		return false;
	if (x + img[(int)imgInd].w <= _x) // x1<y0
		return false;
	if (x >= _x+other->w) // x0>y1
		return false;
	if (y + img[(int)imgInd].h <= _y) // x1<y0
		return false;
	if (y >= _y + other->h) // x0>y1
		return false;
	return true;
}
