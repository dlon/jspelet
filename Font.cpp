#include "Font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "Engine.h"

#include <gl/gl.h>

#include "prof.h"

extern Engine *eng;

void TextEngine::SetFont(Texture *t, unsigned rows, unsigned maxChars)
{
	assert(t); // invalid texture
	rows == 0 ? rows = 1 : 0;

	lpr = maxChars / rows;

	this->maxChars = maxChars;
	this->rows = rows;

	font = t;
	lw = t->w / lpr;
	lh = t->h / rows;
}

void TextEngine::Draw(float x, float y, const char *str)
{
	assert(font); // no font loaded

	Prof(TextEngine_Draw);

	glEnable(eng->render->textureType);
	glBindTexture(eng->render->textureType, font->reserved);

	if (eng->render->rectExt) {
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
	}
	else {
		float xw = float(font->w + font->paddingW);
		float yw = float(font->h + font->paddingH);
		glBegin(GL_QUADS);
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
			glTexCoord2f(s / xw, d / yw);
			glVertex2f(x+i*sLw, y);
			glTexCoord2f(s / xw, (d+(float)lh) / yw );
			glVertex2f(x+i*sLw, y+sLh);
			glTexCoord2f((s+(float)lw) / xw, (d+(float)lh) / yw);
			glVertex2f(x+i*sLw+sLw, y+sLh);
			glTexCoord2f((s+(float)lw) / xw, d / yw);
			glVertex2f(x+i*sLw+sLw, y);
		}
		glEnd();
	}

	glDisable(eng->render->textureType);

#if 0
	// TODO: Improve perhaps? How much slower is this version? (it uses a lot more glBegin/glEnd pairs)
	eng->render->SetBlendMode(RBLEND_NORMAL, true);
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
		SubImage sub;
		sub.t = font;
		sub.tx = s;
		sub.ty = d;
		sub.w = lw;
		sub.h = lh;

		eng->render->DrawSubImageEx(&sub, x+i*sLw, y, 0, scale, scale);
	}
	eng->render->SetBlendMode(RBLEND_NORMAL, false);
#endif
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
