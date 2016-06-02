#include "CSettings.h"
#include <windows.h>

const char *CSettings::appDir = NULL;
char CSettings::appBuffer[1024];
bool CSettings::fullscreen = true;			// default

#define APPDIR_APPNAME "JackeSpelet"

const char *CSettings::GetAppDir()
{
	if (!appDir) {
		size_t pp = strlen(APPDIR_APPNAME); // really static

		if (GetEnvironmentVariable("APPDATA", appBuffer, sizeof(appBuffer) - 1 - pp) != 0) { // success
			strcat(appBuffer, "\\" APPDIR_APPNAME);
			appDir = appBuffer;
		}
		else {
			// use default path
			appDir = ".\\data\\user";
		}
		// construct folder if necessary
		CreateDirectory(appDir, NULL);
	}
	return appDir;
}

void CSettings::ReadSettings(Jack *jack)
{
	std::string s(GetAppDir());
	s += "\\jack.jfg";

	if (!FileExists(s.c_str()))
		return;

	Archiver arc(s.c_str(), true);

	// read cfg
	while (!arc.EndOfFile()) {
		std::string s;
		arc >> s;

		if (!arc.EndOfFile()) {
			if (s == "MusicVol") {
				arc >> jack->musicVol;
			}
			else if (s == "SfxVol") {
				arc >> jack->sfxVol;
			}
			else if (s == "Fullscreen") {
				arc >> fullscreen;
			}
		}
	}
}

bool CSettings::ReadFullscreen()
{
	std::string s(GetAppDir());
	s += "\\jack.jfg";

	if (!FileExists(s.c_str()))
		return fullscreen;

	Archiver arc(s.c_str(), true);

	// read cfg
	while (!arc.EndOfFile()) {
		std::string s;
		arc >> s;

		if (!arc.EndOfFile()) {
			if (s == "MusicVol") {
				float f;
				arc >> f;
			}
			else if (s == "SfxVol") {
				float f;
				arc >> f;
			}
			else if (s == "Fullscreen") {
				arc >> fullscreen;
				break;
			}
		}
	}
	return fullscreen;
}

void CSettings::SaveSettings(Jack *jack)
{
	std::string s(GetAppDir());
	s += "\\jack.jfg";

	Archiver arc(s.c_str(), false);

	// write cfg
	arc << "MusicVol" << jack->musicVol <<
		"SfxVol" << jack->sfxVol <<
		"Fullscreen" << fullscreen;
}

const char * CSettings::GameStateFile()
{
	static std::string sv;
	if (sv.empty()) {
		sv = GetAppDir();
		sv += "\\state.jks";
	}
	return sv.c_str();
}

bool CSettings::GameStateExists()
{
	FILE *fp = fopen(GameStateFile(), "rb");
	if (fp) {
		fclose(fp);
		return true;
	}
	return false;
}
