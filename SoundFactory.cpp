#include "SoundFactory.h"

Sound *SoundFactory::LoadRes(const char *file)
{
	/*
	Sound *ret;
	if (!(ret=AudioDevice::LoadWaveFile(file)))
		return 0;
	*/
	return new Sound(file);
}

void SoundFactory::FreeRes(Sound *t)
{
	//t->Release();
	delete t;
}
