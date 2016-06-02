#include "SoundFactory.h"

sf::SoundBuffer *SoundFactory::LoadRes(const char *file)
{
	sf::SoundBuffer *ret = new sf::SoundBuffer;
	if (!ret->loadFromFile(file))
		delete ret;
		return 0;
	return ret;
}

void SoundFactory::FreeRes(sf::SoundBuffer *t)
{
	delete t;
}
