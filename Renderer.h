#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "Vector2D.h"

#include <GL/gl.h>
#include <GL/glu.h>

typedef unsigned char col_t;

struct col3_t { col_t r,g,b; };
struct col4_t { col_t r,g,b,a; };

class JackeGui;

struct Camera : public Vector2f
{
	float maxX,maxY;

	Camera(float x, float y, float _maxX, float _maxY) : Vector2f(x,y), maxX(_maxX), maxY(_maxY) {}
	Camera() : Vector2f(), maxX(0), maxY(0) {}
};

enum RBLENDMODE {
	RBLEND_NORMAL,
	RBLEND_ADDITIVE,
	RBLEND_REPLACE
};

struct SubImage
{
	sf::Texture* t;
	int tx;
	int ty;
	int w;
	int h;

	/*SubImage(Texture *_t, int _w, int _h, int _tx=0, int _ty=0) : t(_t), tx(_tx), ty(_ty), w(_w), h(_h) {}
	SubImage() {}*/
};

class Renderer
{
	sf::RenderWindow&	window;
	bool		blendTextures;
	RBLENDMODE	rbm;
	bool		vsync;
	sf::Texture*	tempText;
	friend		JackeGui; // FIXME: needs window access to quit
public:
	int			textureType;

	Renderer(sf::RenderWindow& window);

	// public members
	Camera cam;

	// init
	void Close();
	void Resize(unsigned view_w, unsigned view_h, unsigned ortho_w, unsigned ortho_h);

	// pre/post ops
	void BeginFrame();
	void EndFrame();
	void NoViewBegin(); // note: nest within BeginFrame/EndFrame
	void NoViewEnd();
	void TranslateBegin(float x, float y); // ignores camera
	void TranslateEnd();

	// settings
	void SetColor(unsigned char r=255, unsigned char g=255, unsigned char b=255, unsigned char a=255);
	void SetColor3c(col3_t &col3);
	void SetColor4c(col4_t &col4);
	void GetColor3c(col3_t *ret);
	void GetColor4c(col4_t *ret);
	void SetClearColor(unsigned char r=0, unsigned char g=0, unsigned char b=0);
	void SetBlendMode(RBLENDMODE rbm, bool blendTextures = 0);
	void GetBlendMode(RBLENDMODE *rbm, bool *blendTextures);
	
	// textures
	void DrawTexture(sf::Texture *t, float x, float y);
	void DrawTextureEx(sf::Texture *t, float x, float y, float rotDeg, float xscale, float yscale);
	void DrawSubImage(SubImage *s, float x, float y);
	void DrawSubImageEx(SubImage *s, float x, float y, float rotDeg, float xscale, float yscale);
	
	// quad array
	void SetArrayTextureBegin(sf::Texture *t);
	void SetArrayTextureEnd();
	void DrawArray(int num, Vector2f coord[], Vector2f texCoord[], Vector2f sizes[]);

	// hack
	void lfSetTexture(sf::Texture *t);
	void lfSetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
	void lfUnsetTexture();
	void lfBeginQuads();
	void lfEndQuads();
	void lfDrawQuad(float x, float y, float w, float h);
	void lfDrawTexturedQuad(float x, float y, float w, float h, float tx, float ty);

	// primitives
	void DrawRect(float x1, float y1, float w, float h);
	void DrawRectW(float x1, float y1, float w, float h);
	void DrawTrngl(float x1, float y1, float x2, float y2, float x3, float y3);
	void DrawPx(float x, float y);
	void DrawLine(float x1, float y1, float x2, float y2);

	// misc
	bool HasExtension(const char *extStr);
	void SetVsync(bool enabled);
	bool GetVsync();
};

#endif // RENDERER_H
