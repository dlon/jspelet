#pragma once
#ifndef TEXTUREFACTORY_H
#define TEXTUREFACTORY_H

#include "ResourceFactory.h"
#include <SFML/Graphics.hpp>

class TextureFactory : public ResourceFactory<sf::Texture>
{
public:
	TextureFactory() {}
	~TextureFactory() { FreeAll(); }
protected:
	sf::Texture *LoadRes(const char *file);
	void FreeRes(sf::Texture *t);
};

#endif // TEXTUREFACTORY_H
