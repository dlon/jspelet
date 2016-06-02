//	TODO:
//	renderer profiler - monitor texture mem usage, etc
//	#ifndef NDEBUG DrawProfiler()

#include "Renderer.h"
#include "Sprite.h"
#include <assert.h>
#include <stdio.h>

#include "prof.h"

// quicker float operations
// floor float -> int
inline int fint(float v) // not sure of the accuracy here
{
	int res;
	float sign = 0.4999999f;
	__asm {
		mov eax, v
		and eax, 0x80000000
		or  sign, eax
		fld v
		fsub sign
		fistp res;
	}
	return res;
}

// round (default FPU mode) float -> int
// NOTE: use this if you DON'T CARE whether it rounds up/down/nearest
inline int rint(float v)
{
	int res;
	__asm {
		fld v
		fistp res;
	}
	return res;
}
// quicker float operations

// vsync
typedef void (APIENTRY *WGLSWAPINTERVALEXT)(int);
static WGLSWAPINTERVALEXT wglSwapIntervalEXT = NULL; // static member?

bool Renderer::Init(HWND hWnd) {
	assert(!m_hWnd);
	assert(hWnd);

	m_hDC = GetDC(hWnd);
	m_hWnd = hWnd;

	// create pixel format descriptor
	PIXELFORMATDESCRIPTOR pfd = {0};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	//pfd.cColorBits = 32;
	//pfd.cDepthBits = 8;
	pfd.cColorBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int format = ChoosePixelFormat(m_hDC, &pfd);
	assert(format);

	SetPixelFormat(m_hDC, format, &pfd);

	// create WGL context
	m_hGLRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hGLRC);

	// look for required extensions
	if (//1 ||
		(
		!HasExtension("GL_ARB_texture_rectangle") &&
		!HasExtension("GL_NV_texture_rectangle") &&
		!HasExtension("GL_EXT_texture_rectangle")
		))
	{
		//MessageBox(0, "Rectangle extension not available.\nTextures may not be visible.", "OpenGL", MB_ICONERROR);
		rectExt = false;
		textureType = GL_TEXTURE_2D;
		printf("Texture type: GL_TEXTURE_2D\n");
	}
	else {
		rectExt = true;
		textureType = GL_TEXTURE_RECTANGLE_ARB;
		printf("Texture type: GL_TEXTURE_RECTANGLE_ARB\n");
	}

	// vsync
	if (HasExtension("WGL_EXT_swap_control"))
	{
		if (!wglSwapIntervalEXT)
			wglSwapIntervalEXT = (WGLSWAPINTERVALEXT)wglGetProcAddress("wglSwapIntervalEXT");
	}

	// set GL defaults
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glEnable(GL_BLEND);
	//glEnable(GL_COLOR_MATERIAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetBlendMode(RBLEND_NORMAL, false);

	// resize drawing surface
	RECT wsz;
	if (GetClientRect(m_hWnd, &wsz))
		Resize(wsz.right, wsz.bottom, 640, 480);

	return 1;
}

void Renderer::Close() {
	assert(m_hWnd);

	// destroy context & release gfx dc
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hGLRC);
	ReleaseDC(m_hWnd, m_hDC);

	m_hWnd = NULL;
}

void Renderer::Resize(unsigned view_w, unsigned view_h,
					  unsigned ortho_w, unsigned ortho_h) {
	// set client surface
	glViewport(0, 0, view_w, view_h);

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, ortho_w, ortho_h, 0.0f, -5.0f, 5.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static int find_pow_2(int num)
{
	if (num <= 1)
		return num;
	int i;
	for (i=2; i<num; i*=2)
	{
	}
	return i;
}

Texture *Renderer::LoadTextureMem(unsigned char *data, unsigned width, unsigned height, unsigned format)
{
	GLenum glformat;
	switch (format)
	{
	case 1:
		glformat = GL_LUMINANCE; /* grayscale */
		break;
	case 2:
		glformat = GL_LUMINANCE_ALPHA; /* grayscale with alpha channel */
		break;
	case 3:
		glformat = GL_RGB;
		break;
	case 4:
		glformat = GL_RGBA;
		break;
	default:
		assert(0); // unknown format!
	}

	// padding
	int pw=0; 
	int ph=0;
	
	if (!rectExt) {
		pw = find_pow_2(width);
		ph = find_pow_2(height);
		if (pw > ph) ph = pw;
		if (ph > pw) pw = ph;
		pw -= width;
		ph -= height;

		//printf("IMG: %dx%d\nPADDING: %dx%d\nTOTAL: %dx%d\n",width,height,pw,ph,width+pw,height+ph);
	}

	unsigned char *glData;
	if (pw || ph) {
		glData = new unsigned char[(width+pw)*(height+ph)*format];
		for (int i=0; i < height; i++) {
			memcpy(glData + i*(width+pw)*format,
				data + i*width*format,
				width*format);
		}
		// note: uninitialized data
	}
	else
		glData = data;

	/* create GL texture */
	unsigned gltexture;
	glGenTextures(1, &gltexture);
	if (!gltexture) {
		MessageBox(NULL, "Failed to allocate GL texture", 0, MB_ICONERROR);
		return 0;
	}

	// create texture
	glBindTexture(textureType, gltexture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	//glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(textureType,
	             0, /* level-of-detail: no mipmap */
	             format,
	             width + pw,
	             height + ph,
	             0, /* border: 0 or 1 */
	             glformat,
	             GL_UNSIGNED_BYTE,
	             glData);

	Texture *ret = new Texture;
	ret->reserved = gltexture; // internal GL texture
	ret->w = width;
	ret->h = height;
	ret->paddingH = (unsigned)ph;
	ret->paddingW = (unsigned)pw;

	// free padded texture
	if (pw || ph)
		delete[] glData;
	return ret;
}

void Renderer::FreeTexture(Texture *t)
{
	glDeleteTextures(1, &t->reserved);
	delete t;
}

void Renderer::BeginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	if (cam.maxX && cam.x > cam.maxX) cam.x = cam.maxX; // lazy
	if (cam.maxY && cam.y > cam.maxY) cam.y = cam.maxY; // lazy

	glTranslatef(-floor(cam.x), -floor(cam.y), 0.0f);
}

void Renderer::EndFrame() {
	SwapBuffers(m_hDC);
}

void Renderer::NoViewBegin()
{
	glPushMatrix();
	glTranslatef(floor(cam.x), floor(cam.y), 0.0f); // translate away the camera view
}

void Renderer::NoViewEnd()
{
	glPopMatrix();
}

void Renderer::SetClearColor(unsigned char r,
							 unsigned char g,
							 unsigned char b)
{
	glClearColor(	r / 255.0f,
					g / 255.0f,
					b / 255.0f,
					1.0f);
}

void Renderer::SetColor(unsigned char r,
						unsigned char g,
						unsigned char b,
						unsigned char a)
{
	glColor4f(	r / 255.0f,
				g / 255.0f,
				b / 255.0f,
				a / 255.0f);
}

void Renderer::SetColor3c(col3_t &col3)
{
	glColor3f((float)col3.r / 255.0f,
		(float)col3.g / 255.0f,
		(float)col3.b / 255.0f);
}

void Renderer::SetColor4c(col4_t &col4)
{
	glColor4f((float)col4.r / 255.0f,
		(float)col4.g / 255.0f,
		(float)col4.b / 255.0f,
		(float)col4.a / 255.0f);
}

void Renderer::GetColor3c(col3_t *ret)
{
	GLfloat c[4]={0};
	glGetFloatv(GL_CURRENT_COLOR, reinterpret_cast<GLfloat*>(&c));
	ret->r = (col_t)(c[0]*255.0f);
	ret->g = (col_t)(c[1]*255.0f);
	ret->b = (col_t)(c[2]*255.0f);
}

void Renderer::GetColor4c(col4_t *ret)
{
	GLfloat c[4]={0};
	glGetFloatv(GL_CURRENT_COLOR, reinterpret_cast<GLfloat*>(&c));
	ret->r = (col_t)(c[0]*255.0f);
	ret->g = (col_t)(c[1]*255.0f);
	ret->b = (col_t)(c[2]*255.0f);
	ret->a = (col_t)(c[3]*255.0f);
}

void Renderer::SetBlendMode(RBLENDMODE rbm, bool blendTextures) {
	switch (rbm) {
		case RBLEND_ADDITIVE:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		case RBLEND_REPLACE:
			// how?
			break;
		case RBLEND_NORMAL:
		default:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
	}
	this->rbm = rbm;
	this->blendTextures = blendTextures;
}

void Renderer::GetBlendMode(RBLENDMODE *rbm, bool *blendTextures)
{
	*rbm = this->rbm;
	*blendTextures = this->blendTextures;
}

void Renderer::DrawTexture(Texture *t, float x, float y) {
	assert(t);

	Prof(Renderer_DrawTexture);

	if (!blendTextures) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	glEnable(textureType);
	glBindTexture(textureType, t->reserved);

	x = floor(x);
	y = floor(y);
	
	glBegin(GL_TRIANGLE_STRIP);
	if (rectExt) {
		glTexCoord2i(0,0); glVertex2f(x, y);
		glTexCoord2i(0,t->h); glVertex2f(x, y+t->h);
		glTexCoord2i(t->w,0); glVertex2f(x+t->w, y);
		glTexCoord2i(t->w, t->h); glVertex2f(x+t->w, y+t->h);
	}
	else {
		float xw = float(t->w + t->paddingW);
		float yw = float(t->h + t->paddingH);
		glTexCoord2f(0,             0); glVertex2f(x, y);
		glTexCoord2f(0,       t->h/yw); glVertex2f(x, y+(float)t->h);
		glTexCoord2f(t->w/xw,       0); glVertex2f(x+(float)t->w, y);
		glTexCoord2f(t->w/xw, t->h/yw); glVertex2f(x+(float)t->w, y+(float)t->h);
	}
	glEnd();

	glDisable(textureType);
}

void Renderer::DrawSubImage(SubImage *s, float x, float y)
{
	assert(s->t);

	Prof(Renderer_DrawSubImage);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	if (!blendTextures)
		glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(textureType);
	glBindTexture(textureType, s->t->reserved);

	x = floor(x);
	y = floor(y);
	
	if (rectExt) {
		glBegin(GL_QUADS);
		{
			glTexCoord2i(s->tx, s->ty);
			glVertex2f(x, y);
			glTexCoord2i(s->tx, s->ty+s->h);
			glVertex2f(x, y+s->h);
			glTexCoord2i(s->tx+s->w, s->ty+s->h);
			glVertex2f(x+s->w, y+s->h);
			glTexCoord2i(s->tx+s->w, s->ty);
			glVertex2f(x+s->w, y);
		}
		glEnd();
	}
	else {
		float xw = float(s->t->w + s->t->paddingW);
		float yw = float(s->t->h + s->t->paddingH);

		glBegin(GL_QUADS);
		{
			glTexCoord2f(s->tx / xw, s->ty / yw);
			glVertex2f(x, y);
			glTexCoord2f(s->tx / xw, (s->ty+s->h) / yw);
			glVertex2f(x, y+s->h);
			glTexCoord2f((s->tx+s->w) / xw, (s->ty+s->h) / yw);
			glVertex2f(x+s->w, y+s->h);
			glTexCoord2f((s->tx+s->w) / xw, s->ty / yw);
			glVertex2f(x+s->w, y);
		}
		glEnd();
	}
	glDisable(textureType);
	glDisable(GL_CULL_FACE);
}

void Renderer::SetArrayTextureBegin(Texture *t)
{
	if (!blendTextures)
		glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(textureType);
	glBindTexture(textureType, t->reserved);

	tempText = t; //
}

void Renderer::SetArrayTextureEnd()
{
	glDisable(textureType);
}

void Renderer::DrawArray(int num, Vector2f coord[], Vector2f texCoord[], Vector2f sizes[])
{
	if (rectExt) {
		glBegin(GL_TRIANGLE_STRIP);
		{
			for (int i=0; i<num; i++)
			{
				glTexCoord2f(texCoord[i].x, texCoord[i].y);
				glVertex2f(coord[i].x, coord[i].y);
				glTexCoord2f(texCoord[i].x, texCoord[i].y+sizes[i].y);
				glVertex2f(coord[i].x, coord[i].y+sizes[i].y);
				glTexCoord2f(texCoord[i].x+sizes[i].x, texCoord[i].y);
				glVertex2f(coord[i].x+sizes[i].x, coord[i].y);
				glTexCoord2f(texCoord[i].x+sizes[i].x, texCoord[i].y+sizes[i].y);
				glVertex2f(coord[i].x+sizes[i].x, coord[i].y+sizes[i].y);
			}
		}
		glEnd();
	}
	else {
		float xw = float(tempText->w + tempText->paddingW);
		float yw = float(tempText->h + tempText->paddingH);

		glBegin(GL_TRIANGLE_STRIP);
		{
			for (int i=0; i<num; i++)
			{
				glTexCoord2f(texCoord[i].x / xw, texCoord[i].y / yw);
				glVertex2f(coord[i].x, coord[i].y);
				glTexCoord2f(texCoord[i].x / xw, (texCoord[i].y+sizes[i].y) / yw);
				glVertex2f(coord[i].x, coord[i].y+sizes[i].y);
				glTexCoord2f((texCoord[i].x+sizes[i].x) / xw, texCoord[i].y / yw);
				glVertex2f(coord[i].x+sizes[i].x, coord[i].y);
				glTexCoord2f((texCoord[i].x+sizes[i].x) / xw, (texCoord[i].y+sizes[i].y) / yw);
				glVertex2f(coord[i].x+sizes[i].x, coord[i].y+sizes[i].y);
			}
		}
		glEnd();
	}
}

void Renderer::DrawTextureEx(Texture *t, float x, float y, float rotDeg, float xscale, float yscale)
{
	assert(t);

	Prof(Renderer_DrawTextureEx);

	if (!blendTextures)
		glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();

	glEnable(textureType);
	glBindTexture(textureType, t->reserved);

	glTranslatef(floor(x), floor(y), 0);

	// scale
	float nWidth = xscale*t->w;
	float nHeight = yscale*t->h;

	// rotate
	if (rotDeg > 0.0f) {
		glTranslatef(nWidth/2.0f, nHeight/2.0f, 0);
		glRotatef(rotDeg, 0, 0, 1.0f);
		glTranslatef(-nWidth/2.0f, -nHeight/2.0f, 0);
	}

	glScalef(xscale, yscale, 0);

	// draw texture
	if (rectExt) {
		/*
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2i(0, 0);			glVertex2i(0, 0);
			glTexCoord2i(0, t->h);		glVertex2i(0, t->h);
			glTexCoord2i(t->w, 0);		glVertex2i(t->w, 0);
			glTexCoord2i(t->w, t->h);	glVertex2i(t->w, t->h);
		glEnd();
		*/
		glBegin(GL_QUADS);
			glTexCoord2i(0, 0);			glVertex2i(0, 0);
			glTexCoord2i(0, t->h);		glVertex2i(0, t->h);
			glTexCoord2i(t->w, t->h);	glVertex2i(t->w, t->h);
			glTexCoord2i(t->w, 0);		glVertex2i(t->w, 0);
		glEnd();
	}
	else {
		/*
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2i(0, 0);	glVertex2i(0, 0);
			glTexCoord2i(0, 1);	glVertex2i(0, t->h);
			glTexCoord2i(1, 0);	glVertex2i(t->w, 0);
			glTexCoord2i(1, 1);	glVertex2i(t->w, t->h);
		glEnd();
		*/
		glBegin(GL_QUADS);
			glTexCoord2i(0, 0);	glVertex2i(0, 0);
			glTexCoord2i(0, 1);	glVertex2i(0, t->h);
			glTexCoord2i(1, 1);	glVertex2i(t->w, t->h);
			glTexCoord2i(1, 0);	glVertex2i(t->w, 0);
		glEnd();
	}

	glDisable(textureType);
	glPopMatrix();
}

void Renderer::DrawSubImageEx(SubImage *s, float x, float y, float rotDeg, float xscale, float yscale)
{
	assert(s->t);

	Prof(Renderer_DrawSubImageEx);

	if (xscale < 0.0f || yscale < 0.0f)
		glFrontFace(GL_CW);
	else
		glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	if (!blendTextures)
		glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(textureType);
	glBindTexture(textureType, s->t->reserved);

	// pos
	glPushMatrix();
	glTranslatef(floor(x), floor(y), 0);
	
	// scale
	float nWidth = xscale*s->w; // t or s?
	float nHeight = yscale*s->h;

	// rotate
	if (rotDeg > 0.0f) {
		glTranslatef(nWidth/2.0f, nHeight/2.0f, 0);
		glRotatef(rotDeg, 0, 0, 1.0f);
		glTranslatef(-nWidth/2.0f, -nHeight/2.0f, 0);
	}
	glScalef(xscale, yscale, 0);

	// draw image
	if (rectExt) {
		/*
		glBegin(GL_TRIANGLE_STRIP);
		{
			glTexCoord2i(s->tx, s->ty);
			glVertex2i(0, 0);
			glTexCoord2i(s->tx, s->ty+s->h);
			glVertex2i(0, s->h);
			glTexCoord2i(s->tx+s->w, s->ty);
			glVertex2i(s->w, 0);
			glTexCoord2i(s->tx+s->w, s->ty+s->h);
			glVertex2i(s->w, s->h);
		}
		glEnd();
		*/
		glBegin(GL_QUADS);
		{
			glTexCoord2i(s->tx, s->ty);
			glVertex2i(0, 0);
			glTexCoord2i(s->tx, s->ty+s->h);
			glVertex2i(0, s->h);
			glTexCoord2i(s->tx+s->w, s->ty+s->h);
			glVertex2i(s->w, s->h);
			glTexCoord2i(s->tx+s->w, s->ty);
			glVertex2i(s->w, 0);
		}
		glEnd();
	}
	else {
		float xw = float(s->t->w + s->t->paddingW);
		float yw = float(s->t->h + s->t->paddingH);

		/*
		glBegin(GL_TRIANGLE_STRIP);
		{
			glTexCoord2f(s->tx / xw, s->ty / yw);
			glVertex2i(0, 0);
			glTexCoord2f(s->tx / xw, (s->ty+s->h) / yw);
			glVertex2i(0, s->h);
			glTexCoord2f((s->tx+s->w) / xw, s->ty / yw);
			glVertex2i(s->w, 0);
			glTexCoord2f((s->tx+s->w) / xw, (s->ty+s->h) / yw);
			glVertex2i(s->w, s->h);
		}
		glEnd();
		*/
		glBegin(GL_QUADS);
		{
			glTexCoord2f(s->tx / xw, s->ty / yw);
			glVertex2i(0, 0);
			glTexCoord2f(s->tx / xw, (s->ty+s->h) / yw);
			glVertex2i(0, s->h);
			glTexCoord2f((s->tx+s->w) / xw, (s->ty+s->h) / yw);
			glVertex2i(s->w, s->h);
			glTexCoord2f((s->tx+s->w) / xw, s->ty / yw);
			glVertex2i(s->w, 0);
		}
		glEnd();
	}
	glPopMatrix();
	glDisable(textureType);
	glDisable(GL_CULL_FACE);
}

void Renderer::DrawRect(float x, float y, float w, float h) {
	Prof(Renderer_DrawRect);

	x = floor(x);
	y = floor(y);
	w = floor(w);
	h = floor(h);

	/*glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(x, y);
		glVertex2f(x+w, y);
		glVertex2f(x, y+h);
		glVertex2f(x+w, y+h);
	glEnd();*/
	glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x+w, y);
		glVertex2f(x+w, y+h);
		glVertex2f(x, y+h);
	glEnd();
}

void Renderer::DrawRectW(float x, float y, float w, float h) {
	Prof(Renderer_DrawRectW);

	x = floor(x);
	y = floor(y);
	w = floor(w);
	h = floor(h);

	glBegin(GL_LINE_STRIP);
		glVertex2f(x, y);
		glVertex2f(x, y+h);
		glVertex2f(x+w, y+h);
		glVertex2f(x+w, y);
		glVertex2f(x, y);
	glEnd();
}

void Renderer::DrawPx(float x, float y) {
	Prof(Renderer_DrawPx);

	glBegin(GL_POINTS);
	glVertex2f(floor(x), floor(y));
	glEnd();
}

void Renderer::DrawLine(float x1, float y1, float x2, float y2)
{
	Prof(Renderer_DrawLine);

	glBegin(GL_LINES);
	glVertex2f(floor(x1), floor(y1));
	glVertex2f(floor(x2), floor(y2));
	glEnd();
}

bool Renderer::HasExtension(const char *extStr) {
	char *p = (char*)glGetString(GL_EXTENSIONS);
	size_t extNameLen = strlen(extStr);

	assert(p);
	
	while (*p != '\0') {
		size_t n = strcspn(p, " ");
		if ((extNameLen == n) && (strncmp(extStr, p, n) == 0))
			return 1;
		p += (n + 1);
	}
	return 0;
}

void Renderer::SetVsync(bool enabled)
{
	if (wglSwapIntervalEXT != NULL)
		wglSwapIntervalEXT(int(enabled));
	vsync = enabled;
}

bool Renderer::GetVsync()
{
	return vsync;
}

void Renderer::TranslateBegin(float x, float y)
{
	glPushMatrix();
	glTranslatef(floor(cam.x), floor(cam.y), 0.0f);
	glTranslatef(floor(x), floor(y), 0.0f);
}

void Renderer::TranslateEnd()
{
	glPopMatrix();
}

void Renderer::DrawTrngl(float x1, float y1, float x2, float y2, float x3, float y3)
{
	Prof(Renderer_DrawTrngl);

	x1 = floor(x1);
	y1 = floor(y1);
	x2 = floor(x2);
	y2 = floor(y2);
	x3 = floor(x3);
	y3 = floor(y3);

	glBegin(GL_TRIANGLES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();
}

// lf hack
void Renderer::lfSetTexture(Texture *t)
{
	glEnable(textureType);
	glBindTexture(textureType, t->reserved);
	tempText = t;
}

void Renderer::lfUnsetTexture()
{
	glDisable(textureType);
}

void Renderer::lfSetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	SetColor(r,g,b,a);
}

void Renderer::lfBeginQuads()
{
	glBegin(GL_QUADS);
}

void Renderer::lfEndQuads()
{
	glEnd();
}

void Renderer::lfDrawQuad(float x, float y, float w, float h)
{
	y = floor(y);
	x = floor(x);
	w = floor(w);
	h = floor(h);
	glVertex2f(x, y);
	glVertex2f(x+w, y);
	glVertex2f(x+w, y+h);
	glVertex2f(x, y+h);
}

void Renderer::lfDrawTexturedQuad(float x, float y, float w, float h, float tx, float ty)
{
	y = floor(y);
	x = floor(x);
	w = floor(w);
	h = floor(h);
	tx = floor(tx);
	ty = floor(ty);

	if (rectExt) {
		glTexCoord2f(tx, ty);
		glVertex2f(x, y);
		glTexCoord2f(tx+w, ty);
		glVertex2f(x+w, y);
		glTexCoord2f(tx+w, ty+h);
		glVertex2f(x+w, y+h);
		glTexCoord2f(tx, ty+h);
		glVertex2f(x, y+h);
	}
	else {
		float xw = float(tempText->w + tempText->paddingW);
		float yw = float(tempText->h + tempText->paddingH);

		glTexCoord2f(tx / xw, ty / yw);
		glVertex2f(x, y);
		glTexCoord2f((tx+w) / xw, ty / yw);
		glVertex2f(x+w, y);
		glTexCoord2f((tx+w) / xw, (ty+h) / yw);
		glVertex2f(x+w, y+h);
		glTexCoord2f(tx / xw, (ty+h) / yw);
		glVertex2f(x, y+h);
	}
}
