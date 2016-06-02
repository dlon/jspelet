#pragma once
#ifndef INPUT_H
#define INPUT_H

#include <string.h>

enum
{
	KEYUP,
	KEYDOWN		= 0x01,
	KEYPRESSED	= 0x02,
	KEYRELEASED	= 0x04
};

class Input
{
	unsigned char keys[256];
	float mx,my;
public:
	Input() { mx=my=0.0f; memset(keys, 0, 256); }

	bool Check(int i) { return (keys[(unsigned char)i] & KEYDOWN) != 0; }
	bool CheckPressed(int i) { return (keys[(unsigned char)i] & KEYPRESSED) != 0; }
	bool CheckReleased(int i) { return (keys[(unsigned char)i] & KEYRELEASED) != 0; }

	void UnPress() { for (int i=0; i<256; i++) keys[i] &= ~(KEYPRESSED|KEYRELEASED); }

	void HitKey(int i) { if (keys[(unsigned char)i] == KEYUP) keys[(unsigned char)i] = KEYDOWN|KEYPRESSED; }
	void RelKey(int i) { keys[(unsigned char)i] = KEYRELEASED; }

	void SetMousePosition(float x, float y) { mx=x; my=y; }
	void GetMousePosition(float *x, float *y);
};

#endif // INPUT_H
