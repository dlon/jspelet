#pragma once

#include <dsound.h>
#include <vorbis/vorbisfile.h>

class OggFile
{
	LPDIRECTSOUNDBUFFER8 pDsb8;
	OggVorbis_File vorbis;
	bool init;
	int bufferSz;
	bool loopFlag; // not thread safe?
	
	static void NotificationThread(void *user);
	HANDLE hNotificationThread;
	HANDLE hExitSignal;
	HANDLE hBufferOps; // probably not needed

	void Read(int size, int offset);
	bool RegisterNotifyEvents(int refillPart1, int refillPart2);

	void Free();
	void StartThread();
	void CommonPlay();
public:
	OggFile(const char *file);
	OggFile();
	~OggFile();

	void Load(const char *file);
	
	void Play();
	void PlayOnce();
	void Stop();
	void Pause();
	void Resume();
	bool Playing();

	void SetVolume(long vol);
	long GetVolume();

	int GetPositionMsec();
	int GetLengthMsec();
	int SeekPositionMsec(int msec);
};
