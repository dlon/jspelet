#pragma once
#include "Audio.h"
#include <vector>
#include <exception>

class SoundFileNotFound : public std::runtime_error
{
public:
	SoundFileNotFound(const char *file) : std::runtime_error(file) {}
};

class Sound
{
	std::vector<LPDIRECTSOUNDBUFFER8> buffers;

	static float gvol;
	float vol;
	float vLoc;

	void	ReleaseDoubles();
	bool	PlayingInd(int i);
	int		CreateBuffer();
	void	PlayBuffer(int i);
public:
	Sound(const char *file);
	~Sound();

	void			SetVolume(float f);			// f >= 0.0f <= 100.0f
	static void		SetGlobalVolume(float f);	// f >= 0.0f <= 100.0f
	static float	GetGlobalVolume() { return gvol; }

	LPDIRECTSOUNDBUFFER8 ReallyBadFunction(); // dupe buffer - manual handling

	void Play(bool allowMultiple=false);
	void PlayLoc(float dx, float dy, bool allowMultiple=false);
	bool Playing();
	void Stop();
	void Rewind();
};
