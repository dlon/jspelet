//
// TODO: Either a timer to release unused buffers -OR- simply a limit
//

#include "Sound.h"
#include "Jack.h"

float Sound::gvol = 100.0f;

Sound::Sound(const char *file)
{
	LPDIRECTSOUNDBUFFER8 buffer;
	if (! (buffer=AudioDevice::LoadWaveFile(file)) )
		throw SoundFileNotFound(file);
	buffers.push_back(buffer);

	vol = 100.0f;
	vLoc = 1.0f;
}

Sound::~Sound()
{
	for (size_t i=0; i<buffers.size(); i++)
		buffers[i]->Release(); // free
}

bool Sound::PlayingInd(int i)
{
	DWORD st;
	if (FAILED(buffers[i]->GetStatus(&st)))
		return false;
	return (st & DSBSTATUS_PLAYING) != 0;
}

void Sound::ReleaseDoubles()
{
	if (buffers.size() == 1)
		return;

	for (size_t i=1; i<buffers.size(); i++) {
		if (PlayingInd((int)i)) // still have some buffers playing
			return;
	}

	// remove buffers
	for (size_t i=1; i<buffers.size(); i++)
		buffers[i]->Release();
	buffers.erase(buffers.begin()+1, buffers.end());
}

void Sound::SetGlobalVolume(float f)
{
	gvol = f;
}

void Sound::SetVolume(float f)
{
	vol = f;
}

int Sound::CreateBuffer()
{
	LPDIRECTSOUND8			dev			= AudioDevice::GetDevice();
	LPDIRECTSOUNDBUFFER8	tBf8;
	LPDIRECTSOUNDBUFFER		tBf;
	LPDIRECTSOUNDBUFFER		tOrg;

	// duplicate buffer
	buffers[0]->QueryInterface(IID_IDirectSoundBuffer, (LPVOID*)&tOrg);

	if (FAILED(dev->DuplicateSoundBuffer(tOrg, &tBf))) {
		tOrg->Release();
		return -1;
	}
	tBf->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&tBf8);
	tBf->Release();

	// save buffer
	buffers.push_back(tBf8);
	return buffers.size()-1;
}

void Sound::PlayBuffer(int i)
{
	// mix volume
	buffers[i]->SetVolume(-50); buffers[i]->SetVolume(-25); // ms bug workaround
	if (gvol > 0.0f) {
		long mvol = -5000 + long(gvol * vol / 100.0f * vLoc * 50.0f);
		HRESULT hr = buffers[i]->SetVolume(mvol);
	}
	else
		buffers[i]->SetVolume(-10000); // dead 0

	// play
	buffers[i]->Play(0,0,0);
}

LPDIRECTSOUNDBUFFER8 Sound::ReallyBadFunction()
{
	LPDIRECTSOUND8			dev			= AudioDevice::GetDevice();
	LPDIRECTSOUNDBUFFER8	tBf8;
	LPDIRECTSOUNDBUFFER		tBf;
	LPDIRECTSOUNDBUFFER		tOrg;

	// duplicate buffer
	buffers[0]->QueryInterface(IID_IDirectSoundBuffer, (LPVOID*)&tOrg);

	if (FAILED(dev->DuplicateSoundBuffer(tOrg, &tBf))) {
		tOrg->Release();
		return NULL;
	}
	tBf->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&tBf8);
	tBf->Release();

	return tBf8;
}

void Sound::PlayLoc(float dx, float dy, bool allowMultiple)
{
	if (!allowMultiple && Playing())
		return;

	// create dup buffer
	int i;
	if ((i=CreateBuffer()) == -1)
		return; // fail

	// panning
	int px = (int)(min(fabs(dx)/1000.0f, 1.0f)*10000.0f);
	px = max(px - 1000, 0); // center range. FIXME: Good solution?

	if (dx < 0.0f)
		px *= -1;
	buffers[i]->SetPan(px);

	// volume
	float dc = dx*dx + dy*dy; // dist
	float dv = dc / (400.0f*400.0f) * 0.1f; // min = 0.1f?
	vLoc = (1.0f - dv > 0.0f) ? (1.0f - dv) : 0.0f;

	PlayBuffer(i);

	// TODO: moving audio object? (thread or update func)
}

void Sound::Play(bool allowMultiple)
{
	vLoc = 1.0f;
	if (Playing()) {
		if (allowMultiple) {
			int i;
			if ((i=CreateBuffer()) == -1)
				return; // fail
			buffers[i]->SetPan(0);
			PlayBuffer(i);
			return;
		} else
			return;
	}
	ReleaseDoubles();

	buffers[0]->SetPan(0);
	PlayBuffer(0);
}

bool Sound::Playing()
{
	for (size_t i=0; i<buffers.size(); i++) {
		if (PlayingInd((int)i))
			return true;
	}
	return false;
}

void Sound::Stop()
{
	for (size_t i=0; i<buffers.size(); i++)
		buffers[i]->Stop();
	ReleaseDoubles();
	buffers[0]->SetCurrentPosition(0);
}

void Sound::Rewind()
{
	buffers[0]->SetCurrentPosition(0);
}
