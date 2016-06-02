#pragma once
#ifndef SOUNDFACTORY_H
#define SOUNDFACTORY_H

#include "ResourceFactory.h"
#include <SFML/Audio.hpp>

class SoundFactory : public ResourceFactory<sf::SoundBuffer>
{
public:
	~SoundFactory() { FreeAll(); }
protected:
	sf::SoundBuffer* LoadRes(const char *file);
	void FreeRes(sf::SoundBuffer *sb);
};

#endif // SOUNDFACTORY_H
