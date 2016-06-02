#pragma once
#ifndef AUDIO_H
#define AUDIO_H

//#include <windows.h>
//#include <dsound.h>
typedef int LPDIRECTSOUNDBUFFER8; // FIXME-SFML: temporary dummy variable

//typedef IDirectSoundBuffer8 Sound;

extern bool InitDirectSound(HWND hWnd);
extern void CloseDirectSound();

extern bool LoadWaveFile(const char *file, LPDIRECTSOUNDBUFFER8 *ppDsb8);

class AudioDevice
{
	AudioDevice();

	static bool init;
	static LPDIRECTSOUND8 pDS8;
public:
	static bool Init(HWND);
	static void Close();

	static LPDIRECTSOUND8 GetDevice() { return pDS8; }

	static bool CreateWaveBuffer(LPDIRECTSOUNDBUFFER8* ppDsb8,
		WAVEFORMATEX *wfx,
		int bytes_sz,
		//DWORD flags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLFREQUENCY /*| DSBCAPS_GETCURRENTPOSITION2*/);
		DWORD flags = 0); // FIXME-SFML
	static LPDIRECTSOUNDBUFFER8 LoadWaveFile(const char *file);
};

#endif // AUDIO_H
