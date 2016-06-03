#include "Font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "Engine.h"

#include <GL/gl.h>

extern Engine *eng;

void TextEngine::SetFont(sf::Texture *t, unsigned rows, unsigned maxChars)
{
	assert(t); // invalid texture
	rows == 0 ? rows = 1 : 0;

	lpr = maxChars / rows;

	this->maxChars = maxChars;
	this->rows = rows;

	font = t;
	sf::Vector2u sz = t->getSize();
	lw = sz.x / lpr;
	lh = sz.y / rows;
}

void TextEngine::Draw(float x, float y, const char *str)
{
	assert(font); // no font loaded

	sf::Texture::bind(font, sf::Texture::Pixels);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS); // GL_QUAD_STRIP (FIXME: Strip looks weird. Neglegible speed impact? ---- It's because I'm using an extra vertex every call. I'm not sure if it's possible to set the texture coordinates otherwise)
	for (unsigned i=0; str[i] != '\0'; i++)
	{
		// make sure the char is available in the set
		if ( (unsigned char)str[i] > maxChars)
			continue;

		int row = (int)( ((unsigned char)str[i]) / lpr);

		// draw letter
		float s,d;

		s = (float)((unsigned char)str[i]-(row*lpr))*lw;
		d = (float)row*lh;

		float sLw = lw*scale;
		float sLh = lh*scale;

		// draw letter
		glTexCoord2f(s, d);
		glVertex2f(x+i*sLw, y);
		glTexCoord2f(s, d+(float)lh);
		glVertex2f(x+i*sLw, y+sLh);
		glTexCoord2f(s+(float)lw, d+(float)lh);
		glVertex2f(x+i*sLw+sLw, y+sLh);
		glTexCoord2f(s+(float)lw, d);
		glVertex2f(x+i*sLw+sLw, y);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	sf::Texture::bind(NULL);
}

void TextEngine::Printf(float x, float y, const char *str, ...)
{
	char buffer[2048]; // !

	if ( strlen(str) > sizeof(buffer)-1)
		return;

	va_list arg;
	va_start(arg, str);
	vsprintf(buffer, str, arg);
	va_end(arg);

	Draw(x, y, buffer);
}

void TextEngine::Printf(int x, int y, const char *str, ...)
{
	char buffer[2048]; // !

	if ( strlen(str) > sizeof(buffer)-1)
		return;

	va_list arg;
	va_start(arg, str);
	vsprintf(buffer, str, arg);
	va_end(arg);

	Draw(float(x), float(y), buffer);
}

unsigned TextEngine::GetHeight(const char *str)
{
	unsigned ret = 1;
	for (; (*str) != '\0'; str++)
		if (*str == '\n')
			ret++;
	return ret * lh;
}

unsigned TextEngine::GetWidth(const char *str)
{
	unsigned ret = 0;
	unsigned cnt = 0;
	const char *ptr = str;

	// find the widest row
	for (; (*ptr) != '\0'; ptr++)
	{
		if ((*ptr) == '\n') {
			if (cnt > ret) {
				ret = cnt;
				cnt = 0;
			}
		}
		else
			cnt++;
	}

	if (cnt > ret)
		ret = cnt;

	return ret*lw;
}
