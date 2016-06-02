#pragma once
#include <string>
#include "Jack.h"

class CSettings {
	static const char*		appDir;
	static char				appBuffer[1024];
	static bool				fullscreen;
public:
	static const char *		GetAppDir();
	static const char *		GameStateFile();
	static bool				GameStateExists();
	static void				SetFullscreen(bool on)	{ fullscreen = on; }
	static bool				GetFullscreen()			{ return fullscreen; }

	static void ReadSettings(Jack *jack);
	static void SaveSettings(Jack *jack);
	static bool ReadFullscreen();
};
