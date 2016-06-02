#include "OggFile.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "Audio.h"
#include <process.h>

// stream thread

void OggFile::NotificationThread(void *user)
{
	OggFile *ogg = static_cast<OggFile*>(user);

	int updatePart = 2;

	//MessageBox(0,"Thread started",0,0);
	//Sleep(500); // why did I need this?
	do
	{
		/*
		// cursor notifications aren't reliable at all...
		DWORD i = WaitForMultipleObjects(2, ogg->hNotify, FALSE, 100);
		if (i == WAIT_OBJECT_0)
		{
			// fill first half
			ogg->Read(ogg->bufferSz/2, 0);
		}
		else if (i == WAIT_OBJECT_0+1) {
			// fill second half
			ogg->Read(ogg->bufferSz/2, ogg->bufferSz/2);
		}
		*/

		if (WaitForSingleObject(ogg->hBufferOps, 0) == WAIT_OBJECT_0) {
			// hideous version without notifications b/c of shitty drivers
			DWORD cw,cr;
			ogg->pDsb8->GetCurrentPosition(&cr, &cw);
			
			if (updatePart==1 && cw >= (DWORD)ogg->bufferSz/2)
			{
				ogg->Read(ogg->bufferSz/2, 0);
				updatePart = 2;
			}
			else if (updatePart==2 && cw < (DWORD)ogg->bufferSz/2)
			{
				ogg->Read(ogg->bufferSz/2, ogg->bufferSz/2);
				updatePart = 1;
			}
			ReleaseMutex(ogg->hBufferOps);
		} else {
			puts("TAKEN");
		}
	} while (WaitForSingleObject(ogg->hExitSignal, 500) == WAIT_TIMEOUT);
	//MessageBox(0,"Thread closing",0,0);

	ogg->hNotificationThread = 0;
}

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
	if (init)
	{
		Stop();
		pDsb8->Release();
		ov_clear(&vorbis);
		CloseHandle(hExitSignal);
		CloseHandle(hBufferOps);
		init = false;
	}
}

OggFile::~OggFile()
{
	Free();
}

void OggFile::Load(const char *file)
{
	Free();

	if (ov_fopen((char*)file, &vorbis))
		return; // ouch

	// stream buffer
	WAVEFORMATEX wav={0};
	wav.wFormatTag = WAVE_FORMAT_PCM;
	wav.cbSize = sizeof(wav);
	wav.nChannels = vorbis.vi->channels;
	wav.nSamplesPerSec = vorbis.vi->rate;
	wav.wBitsPerSample = 16;
	wav.nBlockAlign = wav.nChannels*2; // 16/8*channels
	wav.nAvgBytesPerSec = wav.nSamplesPerSec * wav.nBlockAlign;

	bufferSz = wav.nAvgBytesPerSec * 2;

	AudioDevice::CreateWaveBuffer(&pDsb8, &wav, bufferSz, DSBCAPS_CTRLVOLUME|DSBCAPS_GLOBALFOCUS|DSBCAPS_GETCURRENTPOSITION2/*|DSBCAPS_CTRLPOSITIONNOTIFY*/);

	// create thread/events (todo: error check)
	hExitSignal = CreateEvent(0, FALSE, FALSE, 0);
	hBufferOps = CreateMutex(NULL, FALSE, NULL);
	init = true;
}

void OggFile::StartThread()
{
	hNotificationThread = (HANDLE)
		_beginthread(NotificationThread,
		0, this
		);
}

#if 0
bool OggFile::RegisterNotifyEvents(int refillPart1, int refillPart2)
{
	LPDIRECTSOUNDNOTIFY8 lpDsNotify;
	HRESULT hr;

	if (SUCCEEDED(
		hr=pDsb8->QueryInterface(IID_IDirectSoundNotify8,
			(LPVOID*)&lpDsNotify))) 
	{ 
		DSBPOSITIONNOTIFY not[2];
		not[0].dwOffset = refillPart1;
		not[0].hEventNotify = hNotify[0];
		not[1].dwOffset = refillPart2;
		not[1].hEventNotify = hNotify[1];
		hr = lpDsNotify->SetNotificationPositions(2, not);
		lpDsNotify->Release();

		if (SUCCEEDED(hr))
			return true;
	}
	return false;
}
#endif

void OggFile::Read(int size, int offset)
{
	HRESULT	hr;
	LPVOID	lpvWrite1;
	LPVOID	lpvWrite2;
	DWORD	dwLength1;
	DWORD	dwLength2;

	hr = pDsb8->Lock(offset, size, &lpvWrite1, &dwLength1, &lpvWrite2, &dwLength2, 0);
	if (hr == DSERR_BUFFERLOST) // retry
	{
		pDsb8->Restore();
		hr = pDsb8->Lock(offset, size, &lpvWrite1, &dwLength1, &lpvWrite2, &dwLength2, 0);
	}

	if (hr != DS_OK)
	{
#ifndef NDEBUG
		if (hr == DSERR_INVALIDCALL)
			puts("Read() == DSERR_INVALIDCALL");
		else if (hr == DSERR_INVALIDPARAM)
			puts("Read() == DSERR_INVALIDPARAM");
		else if (hr == DSERR_PRIOLEVELNEEDED)
			puts ("Read() == DSERR_PRIOLEVELNEEDED");
		else
			puts("Read() != DS_OK");
#endif
		return;
	}

	int sec = 0;
	DWORD i = 0;

	if (loopFlag) {
		while (i < dwLength1)
		{
			int x = ov_read(&vorbis, (char*)lpvWrite1+i, dwLength1-i, 0, 2, 1, &sec);
			if (!x)
				ov_raw_seek(&vorbis, 0);
			else
				i += x;
		}
	}
	else { // untested
		while (i < dwLength1)
		{
			int x = ov_read(&vorbis, (char*)lpvWrite1+i, dwLength1-i, 0, 2, 1, &sec);
			if (!x) {
				memset((char*)lpvWrite1+i, 0, dwLength1-i); // fill rest with silence
				break;
			}
			i += x;
		}
	}

	DWORD i2 = 0;
	if (lpvWrite2)
	{
		int sec2 = 0;

		if (loopFlag) {
			while (i2 < dwLength2)
				i2 += ov_read(&vorbis, (char*)lpvWrite2+i2, dwLength2-i2, 0, 2, 1, &sec2);
		}
		else { // untested
			while (i2 < dwLength2) {
				int x = ov_read(&vorbis, (char*)lpvWrite2+i2, dwLength2-i2, 0, 2, 1, &sec2);
				if (!x) {
					memset((char*)lpvWrite2+i2, 0, dwLength2-i2); // fill rest with silence
					break;
				}
				i2 += x;
			}
		}
	}

	pDsb8->Unlock(lpvWrite1,i,lpvWrite2,i2);
}

void OggFile::CommonPlay()
{
	if (Playing())
		Stop();

	pDsb8->SetCurrentPosition(0);
	Read(bufferSz/2, 0);
	pDsb8->Play(0, 0, DSBPLAY_LOOPING);

	StartThread();
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
	if (!hBufferOps) // 2009-10-10 TODO: I set this to null (end of func). Not sure if this is OK
		return;
	WaitForSingleObject(hBufferOps, INFINITE);

	pDsb8->Stop();

	// close thread
	SetEvent(hExitSignal);
	
	// deadlock hypothesis: thread closes, rendering the handle invalid.
	if (!hNotificationThread)
		puts("hNotificationThread is null!");
	else
		WaitForSingleObject(hNotificationThread, INFINITE);

	ReleaseMutex(hBufferOps);
	hBufferOps = NULL;
}

bool OggFile::Playing()
{
	DWORD status;
	pDsb8->GetStatus(&status);
	return (status & DSBSTATUS_LOOPING) != 0;
}

void OggFile::SetVolume(long vol)
{
	pDsb8->SetVolume(vol);
}

long OggFile::GetVolume()
{
	long dvol;
	pDsb8->GetVolume(&dvol);
	return dvol;
}
