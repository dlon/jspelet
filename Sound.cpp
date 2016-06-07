#include "Sound.h"
#include <cassert>

float Sound::globalVolume = 100.0f;
std::vector<Sound*> Sound::sounds;

void Sound::setGlobalVolume(float volume)
{
	assert(0.0f <= volume <= 100.0f);
	globalVolume = volume;
	for (auto it = sounds.begin(); it != sounds.end(); ++it) {
		(*it)->setVolume((*it)->getVolume());
	}
}

void Sound::setVolume(float volume)
{
	sf::Sound::setVolume(volume * globalVolume/100.0f);
}
