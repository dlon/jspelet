#include "OggFile.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "Audio.h"

// class

OggFile::OggFile(const char *file) : init(false)
{
	Load(file);
}
OggFile::OggFile() : init(false)
{
}

void OggFile::Free()
{
}

OggFile::~OggFile()
{
	Free();
}

void OggFile::Load(const char *file)
{
	Free();
}

void OggFile::Read(int size, int offset)
{
}

void OggFile::CommonPlay()
{
	if (Playing())
		Stop();
}

void OggFile::Play()
{
	loopFlag = true;
	CommonPlay();
}
void OggFile::PlayOnce()
{
	loopFlag = false;
	CommonPlay();
}

void OggFile::Stop()
{
}

bool OggFile::Playing()
{
	return false;
}

void OggFile::SetVolume(long vol)
{
}

long OggFile::GetVolume()
{
	return 0;
}
