#include "TextureFactory.h"

sf::Texture *TextureFactory::LoadRes(const char *file)
{
	sf::Texture *t = new sf::Texture;
	t->loadFromFile(file);
	return t;
}

void TextureFactory::FreeRes(sf::Texture *t)
{
	delete t;
}
