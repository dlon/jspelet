#pragma once
#ifndef GAME_TEXT_H
#define GAME_TEXT_H

#include <SFML/Graphics.hpp>
class Renderer;

class TextEngine {
	unsigned lw;
	unsigned lh;
	unsigned lpr; // per row
	unsigned rows;
	unsigned maxChars;
	sf::Texture *font;
	float scale;
public:
	void SetFont(sf::Texture *t, unsigned rows, unsigned maxChars);
	void SetScale(float f) { scale = f; }
	
	void Draw(float x, float y, const char *str);
	void Printf(float x, float y, const char *str, ...);
	void Printf(int x, int y, const char *str, ...);

	unsigned GetWidth(const char *str);
	unsigned GetHeight(const char *str);

	TextEngine() : font(0), scale(1.0f) {}
};

class MultiTextEngine
{
public:
	MultiTextEngine(TextEngine *smallFont, TextEngine *normalFont, TextEngine *largeFont);
};

#endif
