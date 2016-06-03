#include "Renderer.h"
#include "Sprite.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

Renderer::Renderer(sf::RenderWindow& window) : window(window) {
	textureType = GL_TEXTURE_2D;

	// set GL defaults
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glEnable(GL_BLEND);
	//glEnable(GL_COLOR_MATERIAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetBlendMode(RBLEND_NORMAL, false);
	Resize(640,480, 640,480); // FIXME-SFML: get width and height from window size?
}

void Renderer::Close() {
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

void Renderer::BeginFrame()
{
	//window.clear(); // FIXME-SFML: is it ok to skip this? it seems to call glClearColor, messing up Renderer::SetClearColor

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	if (cam.maxX && cam.x > cam.maxX) cam.x = cam.maxX; // lazy
	if (cam.maxY && cam.y > cam.maxY) cam.y = cam.maxY; // lazy

	glTranslatef(-floor(cam.x), -floor(cam.y), 0.0f);
}

void Renderer::EndFrame() {
	//SwapBuffers(m_hDC); // FIXME-SFML: What the heck is this?
	window.display();
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

void Renderer::DrawTexture(sf::Texture *t, float x, float y) {
	assert(t);

	if (!blendTextures) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	sf::Texture::bind(t, sf::Texture::Pixels);
	glEnable(GL_TEXTURE_2D);

	x = floor(x);
	y = floor(y);

	sf::Vector2u sz = t->getSize();
	
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2i(0,0); glVertex2f(x, y);
	glTexCoord2i(0,sz.y); glVertex2f(x, y+sz.y);
	glTexCoord2i(sz.x,0); glVertex2f(x+sz.x, y);
	glTexCoord2i(sz.x, sz.y); glVertex2f(x+sz.x, y+sz.y);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	sf::Texture::bind(NULL);
}

void Renderer::DrawSubImage(SubImage *s, float x, float y)
{
	assert(s->t);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	if (!blendTextures)
		glColor3f(1.0f, 1.0f, 1.0f);

	sf::Texture::bind(s->t, sf::Texture::Pixels);
	glEnable(GL_TEXTURE_2D);

	x = floor(x);
	y = floor(y);
	
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
	glDisable(GL_TEXTURE_2D);
	sf::Texture::bind(NULL);
	glDisable(GL_CULL_FACE);
}

void Renderer::SetArrayTextureBegin(sf::Texture *t)
{
	if (!blendTextures)
		glColor3f(1.0f, 1.0f, 1.0f);

	sf::Texture::bind(t, sf::Texture::Pixels);
	glEnable(GL_TEXTURE_2D);

	tempText = t; //
}

void Renderer::SetArrayTextureEnd()
{
	glDisable(GL_TEXTURE_2D);
	sf::Texture::bind(NULL);
}

void Renderer::DrawArray(int num, Vector2f coord[], Vector2f texCoord[], Vector2f sizes[])
{
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

void Renderer::DrawTextureEx(sf::Texture *t, float x, float y, float rotDeg, float xscale, float yscale)
{
	assert(t);

	if (!blendTextures)
		glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();

	sf::Texture::bind(t, sf::Texture::Pixels);
	glEnable(GL_TEXTURE_2D);

	glTranslatef(floor(x), floor(y), 0);

	sf::Vector2u sz = t->getSize();

	// scale
	float nWidth = xscale*sz.x;
	float nHeight = yscale*sz.y;

	// rotate
	if (rotDeg > 0.0f) {
		glTranslatef(nWidth/2.0f, nHeight/2.0f, 0);
		glRotatef(rotDeg, 0, 0, 1.0f);
		glTranslatef(-nWidth/2.0f, -nHeight/2.0f, 0);
	}

	glScalef(xscale, yscale, 0);

	// draw texture
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);			glVertex2i(0, 0);
		glTexCoord2i(0, sz.y);		glVertex2i(0, sz.y);
		glTexCoord2i(sz.x, sz.y);	glVertex2i(sz.x, sz.y);
		glTexCoord2i(sz.x, 0);		glVertex2i(sz.x, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	sf::Texture::bind(NULL);
	glPopMatrix();
}

void Renderer::DrawSubImageEx(SubImage *s, float x, float y, float rotDeg, float xscale, float yscale)
{
	assert(s->t);

	if (xscale < 0.0f || yscale < 0.0f)
		glFrontFace(GL_CW);
	else
		glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	if (!blendTextures)
		glColor3f(1.0f, 1.0f, 1.0f);

	sf::Texture::bind(s->t, sf::Texture::Pixels);
	glEnable(GL_TEXTURE_2D);

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
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	sf::Texture::bind(NULL);
	glDisable(GL_CULL_FACE);
}

void Renderer::DrawRect(float x, float y, float w, float h) {
	x = floor(x);
	y = floor(y);
	w = floor(w);
	h = floor(h);

	glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x+w, y);
		glVertex2f(x+w, y+h);
		glVertex2f(x, y+h);
	glEnd();
}

void Renderer::DrawRectW(float x, float y, float w, float h) {
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
	glBegin(GL_POINTS);
	glVertex2f(floor(x), floor(y));
	glEnd();
}

void Renderer::DrawLine(float x1, float y1, float x2, float y2)
{
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
	vsync = enabled;
	window.setVerticalSyncEnabled(enabled);
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
void Renderer::lfSetTexture(sf::Texture *t)
{
	sf::Texture::bind(t, sf::Texture::Pixels);
	glEnable(GL_TEXTURE_2D);
	tempText = t;
}

void Renderer::lfUnsetTexture()
{
	glDisable(GL_TEXTURE_2D);
	sf::Texture::bind(NULL);
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

	glTexCoord2f(tx, ty);
	glVertex2f(x, y);
	glTexCoord2f(tx+w, ty);
	glVertex2f(x+w, y);
	glTexCoord2f(tx+w, ty+h);
	glVertex2f(x+w, y+h);
	glTexCoord2f(tx, ty+h);
	glVertex2f(x, y+h);
}
