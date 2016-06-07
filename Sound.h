#pragma once

#include <SFML/Audio.hpp>
#include <vector>
#include <algorithm>
#include <cassert>

class Sound : public sf::Sound
{
	static float globalVolume; // 0.0f <= globalVolume <= 100.0f (default: 100.0f)
	static std::vector<Sound*> sounds; // unsafe?
	float sfxVol;
public:
	Sound() : sf::Sound(), sfxVol(100.0f) {
		sounds.push_back(this);
	}
	Sound(const Sound &copy) : sf::Sound(copy) {
		sfxVol = copy.sfxVol;
		sounds.push_back(this);
	}
	Sound(const sf::SoundBuffer &buffer) : sf::Sound(buffer), sfxVol(100.0f) {
		sounds.push_back(this);
	}
	virtual ~Sound() { sounds.erase(std::find(sounds.begin(), sounds.end(), this)); }

	static void setGlobalVolume(float volume); // 0.0f <= volume <= 100.0f
	float getVolume() { return sfxVol; }
	void setVolume(float volume); // 0.0f <= volume <= 100.0f
};
