#pragma once
#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H

#include "Sprite.h"

class CollisionObject
{
	friend class CollisionObject;

	float x,y;
	float xof,yof;
	float w,h;
public:
	/*enum
	{
		NONE = 0,
		X1 = 0x01,	// left of this
		X2 = 0x02,	// right of this
		Y1 = 0x04,	// top of this
		Y2 = 0x08,	// bottom of this
		X = X1|X2,
		Y = Y1|Y2
	};
	*/

	//float x,y;
	//float w,h;
	float lSlopeOff;
	float rSlopeOff;

	/*
	float cX,cY; // ceil
	float cW,cH;
	float fX,fY; // floor
	float fW,fH;
	float rSlopeOff,lSlopeOff;
	*/

	float	GetX()				{ return x /*+ xof*/; }	// returns coordinates + offset - FIXME: maybe not such a good idea if X is assumed to be ent->x
	float	GetY()				{ return y /*+ yof*/; }
	float	SetX(float x)		{ this->x = x; return x; }
	float	SetY(float y)		{ this->y = y; return y; }
	float	SetXOff(float xo)	{ xof = xo; return xo; }
	float	SetYOff(float yo)	{ yof = yo; return yo; }
	float	GetXOff()			{ return xof; }	// note: GetX() returns x + offset
	float	GetYOff()			{ return yof; }
	float	GetXnO()			{ return x+xof; } // X + offset
	float	GetYnO()			{ return y+yof; }
	float	GetW()				{ return w; }
	float	GetH()				{ return h; }
	float	SetW(float w)		{ this->w=w; return w; }
	float	SetH(float h)		{ this->h=h; return h; }
	void	SetPos(float x, float y) { this->x=x; this->y=y; }
	void	SetOff(float x, float y) { this->xof=x; this->yof=y; }
	void	SetSize(float w, float h) { this->w=w; this->h=h; }

	bool Test(CollisionObject &other)
	{
		if (other.x+other.xof >= x+xof+w) return false;
		if (other.y+other.yof >= y+yof+h) return false;
		if (other.x+other.xof <= x+xof-other.w) return false;
		if (other.y+other.yof <= y+yof-other.h) return false;
		return true;
	}
	/*int TestIntersection(CollisionObject &other)
	{
		int ret = NONE;
		if (x+w > other.x && x+w <= other.x+other.w) ret |= X2; // could be wrong
		if (y+h > other.y && y+h <= other.y+other.h) ret |= Y2;
		if (x >= other.x && x < other.x+other.w) ret |= X1;
		if (y >= other.y && y < other.y+other.h) ret |= Y1;

		if ((ret & X) == 0 ||(ret & Y) == 0)
			return 0;
		return ret;
	}*/

	void FromSprite(Sprite *spr) {
		x = (float)spr->x;
		y = (float)spr->y;
		w = (float)spr->GetWidth();
		h = (float)spr->GetHeight();
	}

	CollisionObject(float _x=0, float _y=0, float _w=0, float _h=0) : lSlopeOff(0), rSlopeOff(0), xof(0), yof(0), x(_x), y(_y), w(_w), h(_h) {}
};

class CollisionBox
{
public:
	float x,y;
	float w,h;

	bool Test(CollisionBox *other)
	{
		if (other->x >= x+w) return false;
		if (other->y >= y+h) return false;
		if (other->x <= x-other->w) return false;
		if (other->y <= y-other->h) return false;
		return true;
	}
	CollisionBox(float _x=0, float _y=0, float _w=0, float _h=0) : x(_x), y(_y), w(_w), h(_h) {}
};

#endif // COLLISIONOBJECT_H
