#pragma once
#ifndef TILESETID_H
#define TILESETID_H

#include <SFML/Graphics.hpp>

struct Texture;

class TilesetID
{
	TilesetID();
public:
	static const char *GetString(int i);
	static sf::Texture *GetTexture(int i);
	static int Size();
};

#endif // TILESETID_H
