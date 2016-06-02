#include "Audio.h"
#include <stdlib.h>
#include <string.h>
#include "_wavinfo.h"

//static LPDIRECTSOUND8 pDS8;
//static bool dsound_is_init = 0;
//LPDIRECTSOUND8 AudioDevice::pDS8;
bool AudioDevice::init = false;

bool AudioDevice::Init(HWND hWnd)
{
	/*HRESULT hr;
	if (!init)
	{
		// create device
		hr = DirectSoundCreate8(NULL, &pDS8, NULL);
		if (FAILED(hr))
		{
			MessageBox(NULL, "Failed to create sound object", "DirectSound", MB_ICONERROR);
			return false;
		}

		// set cooperative level
		hr = pDS8->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
		if (FAILED(hr))
		{
			MessageBox(NULL, "Failed to set cooperative level", "DirectSound", MB_ICONERROR);
			pDS8->Release();
			return false;
		}
		init = true;
	}
	return init;
	*/
	return true;
}

void AudioDevice::Close()
{
	/*if (init) {
		pDS8->Release();
		init = false;
	}*/
}

bool AudioDevice::CreateWaveBuffer(LPDIRECTSOUNDBUFFER8* ppDsb8,
								   WAVEFORMATEX *wfx,
								   int bytes_sz,
								   DWORD flags)
{
	/*
	DSBUFFERDESC dsbdesc;
	LPDIRECTSOUNDBUFFER pDsb = NULL;
	HRESULT hr;

	// TODO: Add hardware buffering where possible
	// buffer description
	memset(&dsbdesc, '\0', sizeof(dsbdesc));
	dsbdesc.dwSize = sizeof(dsbdesc);
	dsbdesc.dwFlags = flags;
	dsbdesc.dwBufferBytes = bytes_sz;
	dsbdesc.lpwfxFormat = wfx;

	// create it
	hr = pDS8->CreateSoundBuffer(&dsbdesc, &pDsb, NULL);
	if (SUCCEEDED(hr))
	{
		hr = pDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)ppDsb8);
		pDsb->Release();

		if (SUCCEEDED(hr))
			return 1;
	}
	*/
	return 0;
}

LPDIRECTSOUNDBUFFER8 AudioDevice::LoadWaveFile(const char *file)
{
	_wavinfo *wav;
	if (!init)
	{
		//MessageBox(NULL, "LoadWaveFile() called without first initializing a device", "Error", MB_ICONERROR); // FIXME-SFML: cross-platform error message
		return 0;
	}

	if ((wav = wavinfo_load(file)) != NULL)
	{
		// create wave info
		WAVEFORMATEX wfx;
		
		memset(&wfx, '\0', sizeof(wfx));
		wfx.cbSize = sizeof(wfx);
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nChannels = wav->channels;
		wfx.nSamplesPerSec = wav->rate;
		wfx.nBlockAlign = wav->bitspersec / 8 * wfx.nChannels;
		wfx.nAvgBytesPerSec = wav->rate * wfx.nBlockAlign;
		wfx.wBitsPerSample = wav->bitspersec;

		// create buffer
		LPDIRECTSOUNDBUFFER8 pDsb8;
		if (!CreateWaveBuffer(&pDsb8, &wfx, wav->data_len))
		{
			wavinfo_free(wav);
			return 0;
		}

		// fill it with the data
		LPVOID	lpvWrite;
		DWORD	dwLength;
		
		/*
		if (pDsb8->Lock(0, 0,
						&lpvWrite, &dwLength,
						NULL, NULL,
						DSBLOCK_ENTIREBUFFER) == DS_OK)
		{
			memcpy(lpvWrite, wav->data, dwLength);
			pDsb8->Unlock(lpvWrite, dwLength, NULL, 0);
		}
		else
		{
			pDsb8->Release();
			wavinfo_free(wav);
			return 0;
		}
		*/

		wavinfo_free(wav);
		//return pDsb8;
		return 0;
	}
	return 0;
}
