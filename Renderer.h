#ifndef RENDERER_H
#define RENDERER_H

#include "Vector2D.h"
#include <windows.h>
#include "TextureLoader.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

typedef unsigned char col_t;

struct col3_t { col_t r,g,b; };
struct col4_t { col_t r,g,b,a; };

// rectangle textures
#if defined(GL_TEXTURE_RECTANGLE_ARB)
#elif defined(GL_TEXTURE_RECTANGLE_EXT)
#define GL_TEXTURE_RECTANGLE_ARB GL_TEXTURE_RECTANGLE_EXT
#elif defined(GL_TEXTURE_RECTANGLE_NV)
#define GL_TEXTURE_RECTANGLE_ARB GL_TEXTURE_RECTANGLE_NV
#else
#error No GL header files included that contain the rectangle extension!
#endif

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

struct Texture {
	unsigned reserved;
	unsigned w;
	unsigned h;
	unsigned paddingW;
	unsigned paddingH;
};

struct SubImage
{
	Texture *t;
	int tx;
	int ty;
	int w;
	int h;

	/*SubImage(Texture *_t, int _w, int _h, int _tx=0, int _ty=0) : t(_t), tx(_tx), ty(_ty), w(_w), h(_h) {}
	SubImage() {}*/
};

class Renderer : public TextureLoader
{
	HDC			m_hDC;
	HWND		m_hWnd;
	HGLRC		m_hGLRC;
	bool		blendTextures;
	RBLENDMODE	rbm;
	bool		vsync;
	Texture*	tempText;
public:
	bool		rectExt;
	int			textureType;

	Renderer() : m_hWnd(NULL), vsync(false), rectExt(true) {}
	//Renderer(HWND hWnd); // RAII

	// public members
	Camera cam;

	// init
	bool Init(HWND hWnd);
	void Close();
	void Resize(unsigned view_w, unsigned view_h, unsigned ortho_w, unsigned ortho_h);

	// texture loading
	//Texture *LoadTexture(const char *file);
	void FreeTexture(Texture *t);

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
	void DrawTexture(Texture *t, float x, float y);
	void DrawTextureEx(Texture *t, float x, float y, float rotDeg, float xscale, float yscale);
	void DrawSubImage(SubImage *s, float x, float y);
	void DrawSubImageEx(SubImage *s, float x, float y, float rotDeg, float xscale, float yscale);
	
	// quad array
	void SetArrayTextureBegin(Texture *t);
	void SetArrayTextureEnd();
	void DrawArray(int num, Vector2f coord[], Vector2f texCoord[], Vector2f sizes[]);

	// hack
	void lfSetTexture(Texture *t);
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
protected:
	Texture *LoadTextureMem(unsigned char *data, unsigned width, unsigned height, unsigned format);
};

#endif // RENDERER_H
