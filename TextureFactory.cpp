#include "TextureFactory.h"
#include <stdio.h>

sf::Texture *TextureFactory::LoadRes(const char *file)
{
	sf::Texture *t = new sf::Texture;
	if (!t->loadFromFile(file)) {
		delete t;
		return 0;
	}
	return t;
}

void TextureFactory::FreeRes(sf::Texture *t)
{
	delete t;
}
