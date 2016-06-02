#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WAVINFO_H
#define _WAVINFO_H

#include <stdio.h>

// internal struct:
typedef struct {
	char id[5];
	int sz;
	char *data;
} WaveChunk;

typedef struct {
	// do not alter
	char *data;
	int data_len;
	int rate;
	short bitspersec; // correction: per sample?
	short channels;
	
	// internal:
	WaveChunk *__format;
	WaveChunk *__data;
} _wavinfo;

extern _wavinfo *wavinfo_load(const char *file);
extern void wavinfo_free(_wavinfo *w);

// quick function to determine whether a file is a wave
extern int is_wave_file(FILE *file); // 0 = not a wave/failure, >0 = success/file size minus header

#endif //_WAVINFO_H

#ifdef __cplusplus
}
#endif
