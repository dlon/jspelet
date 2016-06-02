#pragma once
#ifndef SOUNDFACTORY_H
#define SOUNDFACTORY_H

#include "Audio.h"
#include "ResourceFactory.h"
#include "Sound.h"

class SoundFactory : public ResourceFactory<Sound>
{
public:
	~SoundFactory() { FreeAll(); }
protected:
	Sound *LoadRes(const char *file);
	void FreeRes(Sound *t);
};

#endif // SOUNDFACTORY_H
