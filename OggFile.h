#pragma once

class OggFile
{
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
